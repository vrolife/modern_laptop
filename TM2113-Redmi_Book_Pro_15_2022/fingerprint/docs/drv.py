# License: LGPL-2.1
import struct
import hmac
import hashlib

import usb.core
import usb.util
import usb.control

from Crypto.Cipher import AES

ev_hello = 1
ev_init_result = 2
ev_arm_result = 3
ev_dead_pixel_report = 4
ev_tls = 5
ev_finger_down = 6
ev_finger_up = 7
ev_image = 8
ev_usb_logs = 9
ev_tls_key = 0x0a
ev_refresh_sensor = 0x20

cmd_init = 1
cmd_arm = 2
cmd_abort = 3
cmd_boot0_req = 4
cmd_tls_init = 5
cmd_tls_data = 6
cmd_indicate_s_state = 8
cmd_get_img = 9
cmd_get_dead_pixel = 10
cmd_get_tls_key = 11
cmd_get_kpi = 12
cmd_set_tls_key = 13
cmd_fuse_device = 16
cmd_fingerprint_sesson_off = 18
cmd_end_enrol = 19
cmd_refresh_sensor = 32
cmd_get_fw_version = 48
cmd_get_hw_unique_id = 49
cmd_flush_keys = 50
cmd_passthru_int_control = 64
cmd_passthru_cs_control = 65
cmd_passthru_int_value_out = 66
cmd_passthru_to_spi = 67
cmd_passthru_int_value_in = 69
cmd_get_state = 80
cmd_get_msos = 170

dev = usb.core.find(idVendor=0x10a5, idProduct=0x9201)
dev.reset()
cfg = dev.get_active_configuration()
intf = cfg[(0,0)]

def match(e):
    return usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_IN
ep = usb.util.find_descriptor(intf, custom_match=match)
assert(ep is not None)

ret = dev.ctrl_transfer(
    bmRequestType=0x40,
    bRequest=cmd_indicate_s_state,
    wValue=0x0010,
    wIndex=0x0000,
    data_or_wLength=0
)
assert(ret == 0)

state = dev.ctrl_transfer(
    bmRequestType=0xc0,
    bRequest=cmd_get_state,
    wValue=0x0000,
    wIndex=0x0000,
    data_or_wLength=72
)
print(f'Version {state[0]}.{state[1]}.{state[2]}.{state[3]}')
print(state)

ret = dev.ctrl_transfer(
    bmRequestType=0x40,
    bRequest=cmd_init,
    wValue=0x0001,
    wIndex=0x0000,
    data_or_wLength=bytearray.fromhex('102f1117')
)
assert(ret == 4)

init_result = ep.read(64)
print('init result', init_result)

tls_key = dev.ctrl_transfer(
    bmRequestType=0xc0,
    bRequest=cmd_get_tls_key,
    wValue=0x0000,
    wIndex=0x0000,
    data_or_wLength=1000
)
tls_key = bytearray(tls_key)
print('tls key', tls_key)


magic, key_offset, key_len, aad_offset, aad_len, hmac_offset, hmac_len = struct.unpack('<IIIIIII', tls_key[:28])
encrypted_key = tls_key[key_offset: key_offset + key_len]
aad = tls_key[aad_offset: aad_offset + aad_len]
sig = tls_key[hmac_offset: hmac_offset + hmac_len]
aad_decoded = aad.decode()

print(encrypted_key, aad_decoded, sig)
assert(aad_decoded == 'FPC TLS Keys\0')

hmac_key = hashlib.sha256(b'FPC_HMAC_KEY\0').digest()
h = hmac.new(hmac_key, digestmod='SHA-256')
h.update(aad)
h.update(encrypted_key)
assert(h.digest() == sig)

print(encrypted_key, aad, sig)

fpc_sealing_key = b'FPC_SEALING_KEY\0'
aes = AES.new(hashlib.sha256(fpc_sealing_key).digest(), AES.MODE_CBC, iv=b'\0'*16)
psk = aes.decrypt(encrypted_key)
print(psk)
print(psk.hex())
