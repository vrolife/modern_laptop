# Redmi Book Pro 14/15 2022 锐龙版 键盘驱动

# Redmi Book Pro 14/15 2022 Ryzen keyboard driver

# Dependences

- make
- acpica-tools
- dmidecode

# Usage

```
git clone https://github.com/vrolife/modern_laptop.git
cd modern_laptop
sh configure.sh
make install # install acpi_override
make grub # update grub
make redmibook_wmi # install wmi driver
make blue8852be # install driver for 8852be blutooth part
```
# Supported products

- TM2107-Redmi_Book_Pro_14_2022 (BIOS Version:RMARB4B0P0808)
- TM2113-Redmi_Book_Pro_15_2022 (BIOS Version:RMARB5B0P0A0A)

# WMI Keymap

- Fn + ESC => KEY_FN_ESC
- Fn + F6  => KEY_PROG1
- Fn + F7  => KEY_PROG2
- Fn + F8  => KEY_PROG3
- Fn + F9  => KEY_PROG4
- Fn + F10 => KEY_FN_F10
- Xiao Ai  => KEY_MEDIA

# Changelog

- 2022年7月18日星期一 下午11:29

  1. 添加蓝牙驱动 [传送](https://github.com/vrolife/modern_laptop/blob/main/TM2113-Redmi_Book_Pro_15_2022/blue8852be/README.md)

- 2022年7月18日星期一 下午6:29

  1. 增加指纹驱动(USB 0x10a5 0x9201) [传送](https://github.com/vrolife/modern_laptop/blob/main/TM2113-Redmi_Book_Pro_15_2022/fingerprint/README.md)

- 2022年7月12日星期二 上午3:25

  1. 修复DSDT一处Buffer越界访问

- 2022年7月10日星期日 下午12:01

  1. 可选 - 将按键KP_Enter映射为Enter。[传送](https://github.com/vrolife/modern_laptop/issues/3)

  2. 修复 - 可能的，PSR导致的内核崩溃。[传送](https://github.com/vrolife/modern_laptop/blob/main/TM2113-Redmi_Book_Pro_15_2022/fix/PSR-crash/README.md)

# More information

[更多信息](https://zhuanlan.zhihu.com/p/530643928)
