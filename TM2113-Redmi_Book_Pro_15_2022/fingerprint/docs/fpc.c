// License: LGPL-2.1
#include <stdio.h>
#include <assert.h>

#include <unistd.h>
#include <poll.h>

#include <libusb-1.0/libusb.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent_ssl.h>
#include <openssl/sha.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/aes.h>

#include "defs.h"

#define HOST_TO_DEVICE 0x40
#define DEVICE_TO_HOST 0xC0

#define BULK_BUFFER_SIZE 64
#define CONTROL_BUFFER_SIZE 1024

struct global;

enum fpc_stage {
    fpc_stage_initial = 0,
    fpc_stage_exit_loop,
    fpc_stage_get_tls_key 
};

struct usbfd {
    struct usbfd* next;
    int fd;
    struct event* event;
    struct global* global;
};

struct global {
    enum fpc_stage stage;

    libusb_context *usb_ctx;
    
    struct libusb_device* dev;
    struct libusb_device_handle* handle;

    struct usbfd* usb_fds;
    struct event* usb_timeout;

    struct event_config *ev_config;
    struct event_base *ev_base;

    unsigned char endpoint;

    struct libusb_transfer* bulk_transfer;
    struct evbuffer* bulk_buffer;
    struct evbuffer_iovec bulk_iovec;

    unsigned char control_buffer[CONTROL_BUFFER_SIZE];

    unsigned char tls_key[SHA256_DIGEST_LENGTH];
    unsigned int tls_key_len;
    SSL_CTX* tls_ctx;
    SSL* tls_conn;
    int tls_pipe[2];
    struct bufferevent* tls_bufev;
    struct event* tls_ev_host_to_device;
    struct event* tls_ev_device_to_conn;
};

void sha256(void* in, size_t sz, unsigned char* out)
{
    SHA256_CTX sha256;
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, in, sz);
    unsigned int osz = 0;
    EVP_DigestFinal(ctx, out, &osz);
    assert(osz == SHA256_DIGEST_LENGTH);
}

int find_endpoint(struct global* global) 
{
    struct libusb_config_descriptor* config_desc;
    int ret = libusb_get_active_config_descriptor(global->dev, &config_desc);
    if (ret < 0) {
        fprintf(stderr, "no active config\n");
        return ret;
    }

    for (int i = 0 ; i < config_desc->bNumInterfaces; ++i) {
        const struct libusb_interface* iface = &config_desc->interface[i];
        for (int d = 0 ; d < iface->num_altsetting; ++d) {
            const struct libusb_interface_descriptor* iface_desc = &iface->altsetting[d];
            for (int e = 0 ; e < iface_desc->bNumEndpoints; ++e) {
                const struct libusb_endpoint_descriptor* ep = &iface_desc->endpoint[e];
                if (ep->bEndpointAddress == 0x82) {
                    global->endpoint = ep->bEndpointAddress;
                    return 0;
                }
            }
        }
    }
    return -1;
}

void start_bulk_transfer(struct global *global);

void update_timeout(struct global* global) {
    if (global->usb_timeout == NULL) {
        return;
    }

    struct timeval tv;
    int ret = libusb_get_next_timeout(global->usb_ctx, &tv);
    if (ret) {
        event_add(global->usb_timeout, &tv);
    }
}

void usb_event_handler(int fd, short event, void* data)
{
     struct global* global = (struct global*)data;

    struct timeval tv = { 0, 0 };
    libusb_handle_events_timeout(global->usb_ctx, &tv);
    update_timeout(global);
}

void fd_added_cb(int fd, short events, void* data)
{
    struct global* global = (struct global*)data;
    struct usbfd *usbfd = (struct usbfd*)malloc(sizeof(struct usbfd));
    if (usbfd == NULL) {
        fprintf(stderr, "fd_added_cb out of memory\n");
        return;
    }
    memset(usbfd, 0, sizeof(struct usbfd));
    short e = EV_PERSIST;
    if (events & POLLIN) {
        e |= EV_READ;
    }
    if (events & POLLOUT) {
        e |= EV_WRITE;
    }
    usbfd->fd = fd;
    usbfd->next = global->usb_fds;
    usbfd->global = global;
    usbfd->event = event_new(global->ev_base, fd, e, usb_event_handler, global);
    event_add(usbfd->event, NULL);
    update_timeout(global);

    global->usb_fds = usbfd;
}

void fd_removed_cb(int fd, void* data)
{
    struct global* global = (struct global*)data;

    if (global->usb_fds == NULL)
        return;

    if (fd == global->usb_fds->fd) {
        event_del(global->usb_fds->event);
        global->usb_fds = global->usb_fds->next;
    } else {
        struct usbfd* prev = global->usb_fds;
        struct usbfd* cur = global->usb_fds->next;
        while(cur) {
            if (fd == cur->fd) {
                event_del(cur->event);
                prev->next = cur->next;
                cur = prev->next;
            } else {
                cur = cur->next;
            }
        }
    }
}

void handle_fpc_init_result(struct global* global, struct fpc_event* ev)
{
    printf("got init result\n");
    evbuffer_drain(global->bulk_buffer, ntohl(ev->len));

    global->stage = fpc_stage_get_tls_key;
    event_base_loopbreak(global->ev_base);
}

void handle_fpc_tls_data(struct global* global, struct fpc_event* ev)
{
    evbuffer_drain(global->bulk_buffer, sizeof(struct fpc_event));

    int sz = evbuffer_write(global->bulk_buffer, global->tls_pipe[0]);
    evbuffer_drain(global->bulk_buffer, sz);

    if (evbuffer_get_length(global->bulk_buffer) > 0) {
        event_add(global->tls_ev_device_to_conn, NULL);
    }
}

void handle_fpc_event(struct libusb_transfer *transfer)
{
    struct global* global = (struct global*)transfer->user_data;

    global->bulk_iovec.iov_len = transfer->actual_length;
    evbuffer_commit_space(global->bulk_buffer, &global->bulk_iovec, 1);

    struct fpc_event *ev = (struct fpc_event*)evbuffer_pullup(global->bulk_buffer, 12);
    uint32_t len = ntohl(ev->len);

    if (ev == NULL || len != evbuffer_get_length(global->bulk_buffer)) {
        start_bulk_transfer(global);
        return;
    }

    uint32_t code = ntohl(ev->code);

    switch(code) {
        case ev_hello:break;
        case ev_init_result:
            handle_fpc_init_result(global, ev);
            break;
        case ev_arm_result:break;
        case ev_dead_pixel_report: break;
        case ev_tls:
            handle_fpc_tls_data(global, ev);
            break;
        case ev_finger_down:
            break;
        case ev_finger_up: break;
        case ev_image:
            break;
        case ev_usb_logs: break;
        case ev_tls_key: break;
        case ev_refresh_sensor: break;
    }
    start_bulk_transfer(global);
}

void start_bulk_transfer(struct global *global)
{
    evbuffer_reserve_space(global->bulk_buffer, BULK_BUFFER_SIZE, &global->bulk_iovec, 1);
    
    libusb_fill_bulk_transfer(
        global->bulk_transfer, global->handle, 
        global->endpoint, 
        global->bulk_iovec.iov_base, BULK_BUFFER_SIZE, 
        handle_fpc_event, global, 0);

    libusb_submit_transfer(global->bulk_transfer);
}

int fpc_get_tls_key(struct global* global)
{
    global->stage = fpc_stage_initial;

    int ret = libusb_control_transfer(global->handle, DEVICE_TO_HOST, cmd_get_tls_key, 0, 0, global->control_buffer, 1000, 1000);
    if (ret < 0)
    {
        fprintf(stderr, "cmd_get_tls_key failed\n");
        abort();
    }

    struct fpc_tls_key* hdr = (struct fpc_tls_key*)global->control_buffer;
    if (hdr->magic != 0x0dec0ded
        || (hdr->aad_offset + hdr->aad_len) > ret
        || (hdr->key_offset + hdr->key_len) > ret
        || (hdr->sig_offset + hdr->sig_len) > ret) 
    {
        fprintf(stderr, "invalid tls key packet\n");
        abort();
    }

    if (memcmp("FPC TLS Keys", hdr->data + hdr->aad_offset, 13) != 0) {
        fprintf(stderr, "bad aad failed\n");
        abort();
    }

    // check signature
    unsigned char hmac_key[SHA256_DIGEST_LENGTH];
    sha256("FPC_HMAC_KEY", 13, hmac_key);

    char sig[SHA256_DIGEST_LENGTH];

    // size_t sig_len = 0;
    // EVP_PKEY *pkey = EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, NULL, hmac_key, SHA256_DIGEST_LENGTH);
    // EVP_MD_CTX* hmac = EVP_MD_CTX_new();
    // EVP_DigestSignInit(hmac, NULL, EVP_sha256(), NULL, pkey);
    // EVP_DigestSignUpdate(hmac, hdr->data + hdr->aad_offset, hdr->aad_len);
    // EVP_DigestSignUpdate(hmac, hdr->data + hdr->key_offset, hdr->key_len);
    // EVP_DigestSignFinal(hmac, sig, &sig_len);
    // EVP_MD_CTX_free(hmac);
    // EVP_PKEY_free(pkey);

    unsigned int sig_len = 0;
    HMAC_CTX* hmac = HMAC_CTX_new();
    HMAC_Init(hmac, hmac_key, SHA256_DIGEST_LENGTH, EVP_sha256());
    HMAC_Update(hmac, hdr->data + hdr->aad_offset, hdr->aad_len);
    HMAC_Update(hmac, hdr->data + hdr->key_offset, hdr->key_len);
    HMAC_Final(hmac, sig, &sig_len);
    HMAC_CTX_free(hmac);

    assert(memcmp(sig, hdr->data + hdr->sig_offset, SHA256_DIGEST_LENGTH) == 0);

    unsigned char sealing_key[SHA256_DIGEST_LENGTH];
    sha256("FPC_SEALING_KEY", 16, sealing_key);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    int out_len = 0;
    EVP_CipherInit(ctx, EVP_aes_256_cbc(), sealing_key, 0, 0);

    size_t decrypted = 0;
    while(decrypted < hdr->key_len) {
        EVP_CipherUpdate(ctx, global->tls_key + decrypted, &out_len, hdr->data + hdr->key_offset + decrypted, hdr->key_len - decrypted);
        decrypted += out_len;
    }

    EVP_CipherFinal(ctx, global->tls_key + out_len, &out_len);
    EVP_CIPHER_CTX_free(ctx);

    global->tls_key_len = SHA256_DIGEST_LENGTH;

    // tls_init
    ret = libusb_control_transfer(
        global->handle, HOST_TO_DEVICE, 
        cmd_tls_init, 0x0001, 0, NULL, 0, 0);
    if (ret < 0)
    {
        fprintf(stderr, "cmd_init failed\n");
        abort();
    }
}

unsigned int psk_server_cb(SSL* ssl, const char* identity, unsigned char *psk, unsigned int max_psk_len)
{
    struct global* global = (struct global*)SSL_get_app_data(ssl);
    assert(max_psk_len >= global->tls_key_len);
    memcpy(psk, global->tls_key, global->tls_key_len);
    return global->tls_key_len;
}

void ev_host_to_device_cb(int fd, short events, void* data)
{
    struct global* global = (struct global*)data;
    static size_t rsz = 0;
    if (rsz == 0) {
        rsz = 64;
//         FILE* fp = fopen("../b64.bin", "rb");
//         fread(global->control_buffer, 64, 1, fp);
//         fclose(fp);
    } else if (rsz == 64) {
        rsz = 15;
//         FILE* fp = fopen("../b15.bin", "rb");
//         fread(global->control_buffer, 64, 1, fp);
//         fclose(fp);
    } else if (rsz == 15) {
        rsz = 9;
//         FILE* fp = fopen("../b9.bin", "rb");
//         fread(global->control_buffer, 64, 1, fp);
//         fclose(fp);
    } else {
        abort();
    }
    
    ssize_t sz = read(fd, global->control_buffer, rsz);
    if (sz <= 0) {
        abort();
    }
    
    int ret = libusb_control_transfer(
        global->handle, 
        HOST_TO_DEVICE, cmd_tls_data, 0x0001, 0, global->control_buffer, rsz, 1000);
    if (ret < 0)
    {
        fprintf(stderr, "cmd_init failed\n");
        abort();
    }
}

void ev_device_to_conn(int fd, short events, void* data)
{
    struct global* global = (struct global*)data;
    
    int sz = evbuffer_write(global->bulk_buffer, global->tls_pipe[0]);
    evbuffer_drain(global->bulk_buffer, sz);

    if (evbuffer_get_length(global->bulk_buffer) > 0) {
        event_add(global->tls_ev_device_to_conn, NULL);
    }
}

void tls_read_cb(struct bufferevent *bev, void *data)
{
    printf("tls read\n");
}

void tls_event_cb(struct bufferevent *bev, short what, void *data)
{
    printf("tls event %d\n", what);
    switch(what) {
        case BEV_EVENT_CONNECTED:
            printf("tls event connected\n");
            break;
        case BEV_EVENT_READING:
            printf("tls event reading\n");
            break;
        case BEV_EVENT_WRITING:
            printf("tls event writing\n");
            break;
        case BEV_EVENT_TIMEOUT:
            printf("tls event timeout\n");
            break;
        case BEV_EVENT_EOF:
            printf("tls event eof\n");
            break;
        case BEV_EVENT_ERROR:
            printf("tls event error\n");
            break;
        default: break;
    }
}

int main(int argc, char const *argv[])
{
    struct global global;
    memset(&global, 0, sizeof(global));

    global.ev_config = event_config_new();
    global.ev_base = event_base_new_with_config(global.ev_config);
    global.bulk_buffer = evbuffer_new();
    global.bulk_transfer = libusb_alloc_transfer(0);

    global.tls_ctx = SSL_CTX_new(TLS_server_method());
    if (global.tls_ctx == NULL) {
        fprintf(stderr, "SSL_CTX_new failed\n");
        goto cleanup;
    }

    SSL_CTX_set_options(global.tls_ctx, SSL_OP_NO_COMPRESSION);
    SSL_CTX_use_psk_identity_hint(global.tls_ctx, NULL); // no ServerKeyExchange
    SSL_CTX_set_psk_server_callback(global.tls_ctx, psk_server_cb);

    int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, global.tls_pipe);
    if (ret < 0) {
        fprintf(stderr, "socketpair failed\n");
        goto cleanup;
    }
    evutil_make_socket_nonblocking(global.tls_pipe[0]);
    evutil_make_socket_nonblocking(global.tls_pipe[1]);
    
    global.tls_conn = SSL_new(global.tls_ctx);
    if (global.tls_conn == NULL) {
        fprintf(stderr, "SSL_new failed\n");
        goto cleanup;
    }
    SSL_set_mode(global.tls_conn, SSL_MODE_SEND_SERVERHELLO_TIME);
    SSL_set_app_data(global.tls_conn, &global);
        
    global.tls_bufev = bufferevent_openssl_socket_new(
        global.ev_base, 
        global.tls_pipe[1], 
        global.tls_conn, 
        BUFFEREVENT_SSL_ACCEPTING, 0);
    bufferevent_setcb(global.tls_bufev, tls_read_cb, NULL, tls_event_cb, &global);

    global.tls_ev_host_to_device = event_new(
        global.ev_base, global.tls_pipe[0], 
        EV_READ | EV_PERSIST, 
        ev_host_to_device_cb, &global);
    event_add(global.tls_ev_host_to_device, NULL);

    global.tls_ev_device_to_conn = event_new(
        global.ev_base, global.tls_pipe[0],
        EV_WRITE | EV_PERSIST,
        ev_device_to_conn, &global);

    ret = libusb_init(&global.usb_ctx);
    if (ret < 0)
    {
        fprintf(stderr, "libusb_init failed\n");
        goto cleanup;
    }

    libusb_set_pollfd_notifiers(global.usb_ctx, fd_added_cb, fd_removed_cb, &global);

    if (libusb_pollfds_handle_timeouts(global.usb_ctx) == 0) {
        global.usb_timeout = event_new(global.ev_base, 0, EV_TIMEOUT, usb_event_handler, &global);
    }

    global.handle = libusb_open_device_with_vid_pid(global.usb_ctx, 0x10a5, 0x9201);
    if (global.handle == NULL) {
        fprintf(stderr, "unable open device\n");
        goto cleanup;
    }

    global.dev = libusb_get_device(global.handle);

    struct libusb_device_descriptor desc;
    libusb_get_device_descriptor(libusb_get_device(global.handle), &desc);

    unsigned char string[256];

    if (desc.iProduct)
    {
        ret = libusb_get_string_descriptor_ascii(global.handle, desc.iProduct, string, sizeof(string));
        if (ret > 0)
            printf("Product: %s\n", string);
    }

    libusb_reset_device(global.handle);

    // find endpoint
    ret = find_endpoint(&global);
    if (ret < 0)
    {
        fprintf(stderr, "endpoint not found\n");
        goto cleanup;
    }

    // cmd_indicate_s_state
    ret = libusb_control_transfer(global.handle, HOST_TO_DEVICE, cmd_indicate_s_state, 0x0010, 0, NULL, 0, 1000);
    if (ret < 0)
    {
        fprintf(stderr, "cmd_indicate_s_state failed\n");
        goto cleanup;
    }

    // cmd_get_state
    ret = libusb_control_transfer(global.handle, DEVICE_TO_HOST, cmd_get_state, 0, 0, global.control_buffer, 72, 1000);
    if (ret < 0)
    {
        fprintf(stderr, "cmd_get_state failed\n");
        goto cleanup;
    }

    printf("Version %d.%d.%d.%d\n", 
        global.control_buffer[0], global.control_buffer[1], 
        global.control_buffer[2], global.control_buffer[2]);

    // bulk in
    start_bulk_transfer(&global);

    // cmd_init
    global.control_buffer[0] = 0x10;
    global.control_buffer[1] = 0x2f;
    global.control_buffer[2] = 0x11;
    global.control_buffer[3] = 0x17;
    ret = libusb_control_transfer(global.handle, HOST_TO_DEVICE, cmd_init, 0x0001, 0, global.control_buffer, 4, 1000);
    if (ret < 0)
    {
        fprintf(stderr, "cmd_init failed\n");
        goto cleanup;
    }

    update_timeout(&global);

    while (ret != -1 && global.stage != fpc_stage_exit_loop) 
    {
        switch (global.stage) {
            case fpc_stage_initial:
                ret = event_base_dispatch(global.ev_base);
                break;
            case fpc_stage_get_tls_key:
                ret = fpc_get_tls_key(&global);
                break;
            default: break;
        }
    }

cleanup:
    if (global.usb_ctx) {
        libusb_exit(global.usb_ctx);
    }
    if (global.bulk_transfer) {
        libusb_free_transfer(global.bulk_transfer);
    }
    if (global.ev_config) {
        event_config_free(global.ev_config);
    }
    if (global.ev_base) {
        event_base_free(global.ev_base);
    }
    if (global.usb_fds) {
        fprintf(stderr, "fatal: usb fds leaked\n");
        abort();
    }
    if (global.bulk_buffer) {
        evbuffer_free(global.bulk_buffer);
    }
    if (global.tls_ctx) {
        SSL_CTX_free(global.tls_ctx);
    }
    if (global.tls_conn) {
        SSL_free(global.tls_conn);
    }
    if (global.tls_pipe[0] != -1) {
        close(global.tls_pipe[0]);
    }
    if (global.tls_pipe[1] != -1) {
        close(global.tls_pipe[1]);
    }
    if (global.tls_bufev) {
        bufferevent_free(global.tls_bufev);
    }
    return ret;
}
