// License: LGPL-2.1
#ifndef __fpc_h__
#define __fpc_h__

enum fpc_event_code {
    ev_hello = 1,
    ev_init_result = 2,
    ev_arm_result = 3,
    ev_dead_pixel_report = 4,
    ev_tls = 5,
    ev_finger_down = 6,
    ev_finger_up = 7,
    ev_image = 8,
    ev_usb_logs = 9,
    ev_tls_key = 0x0a,
    ev_refresh_sensor = 0x20
};

// bRequest
enum fpc_command {
    cmd_init = 1,
    cmd_arm = 2,
    cmd_abort = 3,
    cmd_boot0_req = 4,
    cmd_tls_init = 5,
    cmd_tls_data = 6,
    cmd_indicate_s_state = 8,
    cmd_get_img = 9,
    cmd_get_dead_pixel = 10,
    cmd_get_tls_key = 11,
    cmd_get_kpi = 12,
    cmd_set_tls_key = 13,
    cmd_fuse_device = 16,
    cmd_fingerprint_sesson_off = 18,
    cmd_end_enrol = 19,
    cmd_refresh_sensor = 32,
    cmd_get_fw_version = 48,
    cmd_get_hw_unique_id = 49,
    cmd_flush_keys = 50,
    cmd_passthru_int_control = 64,
    cmd_passthru_cs_control = 65,
    cmd_passthru_int_value_out = 66,
    cmd_passthru_to_spi = 67,
    cmd_passthru_int_value_in = 69,
    cmd_get_state = 80,
    cmd_get_msos = 170
};

// bulk in
struct __attribute__ ((packed)) fpc_event {
    unsigned int code;
    unsigned int len;
    unsigned int unknown;
};

struct __attribute__ ((packed)) fpc_tls_key {
    unsigned char data[0];
    unsigned int magic;
    unsigned int key_offset;
    unsigned int key_len;
    unsigned int aad_offset;
    unsigned int aad_len;
    unsigned int sig_offset;
    unsigned int sig_len;
};

#endif
