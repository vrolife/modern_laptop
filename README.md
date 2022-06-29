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
```
# Keymap

- Fn + ESC => KEY_FN_ESC
- Fn + F6  => KEY_PROG1
- Fn + F7  => KEY_PROG2
- Fn + F8  => KEY_PROG3
- Fn + F9  => KEY_PROG4
- Fn + F10 => KEY_FN_F10
- Xiao Ai  => KEY_MEDIA

[更多信息|More information](https://zhuanlan.zhihu.com/p/530643928)

