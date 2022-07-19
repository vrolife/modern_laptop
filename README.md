# Redmi Book Pro 14/15 2022 锐龙版 键盘驱动

# Redmi Book Pro 14/15 2022 Ryzen keyboard driver

# Dependences

- make
- acpica-tools
- dmidecode

# 升级

如果之前`make install`安装过/boot/acpi_override，升级安装新版的话，会出现这样的错误

```
patching file dsdt.dsl
Reversed (or previously applied) patch detected!  Assume -R? [n] y
Hunk #3 FAILED at 5471.
Hunk #4 FAILED at 5479.
2 out of 4 hunks FAILED -- saving rejects to file dsdt.dsl.rej
```

这时需要启动到Grub启动菜单，在启动选项按E编辑，删除initrd后面的acpi_override部分。接着按F10启动进系统安装新版补丁。注意这时键盘是无效的，请自备外置键盘。

无痛升级在此轮`2022年7月19日星期二 下午12:39`更新后支持

# 卸载

键盘补丁：编辑文件`/etc/default/grub`，删除`GRUB_EARLY_INITRD_LINUX_CUSTOM="acpi_override"`后`update-initramfs -k all`
功能键：`sudo dkms remove redmibook_wmi/x.x.x`
蓝牙驱动：`sudo dkms remove blue8852be/x.x.x`
网卡驱动： `sudo dkms remove rtl8852be/x.x.x`

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

安装后，目录下的original_dsdt.dat是原版DSDT,请保留此文件用于将来升级DSDT补丁。

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

- 2022年7月19日星期二 下午12:39

  1. 支持从旧版升级

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
