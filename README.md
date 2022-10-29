
## Redmi Book Pro 14/15 2022 锐龙版 Linux 驱动

## Redmi Book Pro 14/15 2022 Ryzen Linux Driver

[English](https://github-com.translate.goog/vrolife/modern_laptop?_x_tr_sl=auto&_x_tr_tl=en&_x_tr_hl=en-US&_x_tr_pto=wapp)

## 提示

安装依赖包需要网络连接。所以请提前接入互联网。可能的办法有：

1. 使用Linux能识别的USB外置网卡。

2. 通过USB共享你的Android/iPhone的移动网络.

## 更新BIOS后

1. 关闭安全启动

2. 如果文件`original_dsdt.dat`存在，请将之删除

## 依赖包

- dkms
- bash
- make
- acpica-tools
- dmidecode
- mokutil

## 发行版

此仓库中的全部驱动都是基于Ubuntu 22.04 + 5.17 内核开发。所以，建议使用Ubuntu 22.04。

Ubuntu 22.04 可通过安装包linux-image-oem-22.04获得5.17内核。

支持的发行版：

1. Ubuntu 22.04 LTS

2. ArchLinux/Manjaro

3. openSUSE Tumbleweed

4. Fedora

## 组件

Drivers:

- redmibook_wmi 功能键 [Keymap](https://github.com/vrolife/modern_laptop/blob/main/drivers/redmibook_wmi/README.md)
- redmibook_kbd_backlight 键盘背光
- redmibook_dmic 麦克风  - After Linux 5.19 or linux-oem-5.17.0-1014, microphone driven by ACPI patch. Do not install this driver. [链接](https://github.com/vrolife/modern_laptop/blob/main/fixes/acpi/README.md)
- [fingerprint](https://github.com/vrolife/fingerprint-ocv) 指纹 - For USB 10a5:9201(FPC FPC Sensor Controller) only
- blue8852be 蓝牙
- rtl8852be WIFI
- auto-brightness 自动亮度

Fixes:

- acpi - Drive the keyboard(Linux<5.19.11) and microphone(>=Ubuntu Linux oem 5.17.0-1014, >=Mainline Linux 5.19) without kmod. Fix power management. [Detail](https://github.com/vrolife/modern_laptop/blob/main/fixes/acpi/README.md)
- disable-PSR

## 安装

```bash
git clone https://github.com/vrolife/modern_laptop.git
cd modern_laptop
sudo /bin/sh install.sh acpi                      ## apply ACPI patch
sudo /bin/sh install.sh blue8852be                ## 8852be blutooth
sudo /bin/sh install.sh rtl8852be                 ## 8852be wifi
sudo /bin/sh install.sh fingerprint               ## fingerprint
sudo /bin/sh install.sh redmibook_wmi             ## function keys
sudo /bin/sh install.sh redmibook_kbd_backlight   ## keyboard backlight
sudo /bin/sh install.sh redmibook_dmic            ## microphone - After Linux 5.19 or linux-oem-5.17.0-1014, microphone driven by ACPI patch. Do not install this driver.
sudo /bin/sh install.sh disable-PSR               ## disable PSR
```

安装后，目录下的original_dsdt.dat是原版DSDT,请保留此文件用于将来升级DSDT补丁。

## 卸载

`sudo /bin/sh uninstall.sh driver_name`

## 支持机型

- TM2113-Redmi_Book_Pro_15_2022

    BIOS Version: RMARB5B0P0A0A RMARB5B0P0B0B

    ICC Profile: [Link](https://github.com/vrolife/modern_laptop/issues/38)

- TM2107-Redmi_Book_Pro_14_2022

    BIOS Version: RMARB4B0P0808 RMARB4B0P0B0B

## 已知问题

- 目前内核（<6.0）不支持 Yellow Carp PSR。 [链接](https://lore.kernel.org/all/20220510204508.506089-15-dingchen.zhang@amd.com/T/)

- S0休眠耗电，大约3.7%每小时。S4/S5 休眠唤醒ACPI报错。混合休眠不工作。

## 变更

- 2022/10/4

  1. [Fingerprint driver source code](https://github.com/vrolife/fingerprint-ocv)

- 2022-09-07

  1. rtl8852be for Linux 6.0-rc4

- 2022-09-01

  1. 在`Linux 5.19`和 Ubuntu 的`linux-oem-5.17 5.17.0-1014`中，通过ACPI补丁驱动麦克风。[链接](https://github.com/vrolife/modern_laptop/blob/main/fixes/acpi/README.md)

- 2022-08-26

  1. rtl8852be 1.0.2 支持 Linux 5.19

- 2022-08-25

  1. ACPI 补丁支持 TM2107-Redmi_Book_Pro_14_2022/RMARB5B0P0B0B [链接](https://github.com/vrolife/modern_laptop/issues/14)

- 2022-08-23

  1. 支持`openSUSE Tumbleweed`

- 2022年8月9日

  1. 添加自动亮度调节程序。[链接](https://github.com/vrolife/modern_laptop/blob/main/drivers/auto-brightness/README.md)

  2. 添加 KDE 平滑亮度补丁。

- 2022年8月7日

  1. 改进安装方式

  2. 指纹新增fingerpp2.archlinux。[链接](https://github.com/vrolife/modern_laptop/blob/main/drivers/fingerprint/README.archlinux.md)

  3. redmibook_dmic支持TM2107

  4. redmibook_kbd_backlight支持自动加载。不必写入/etc/modules

- 2022年8月1日

  1. 新增麦克风驱动（Redmibook Pro 15 2022 & Linux 5.17）。[链接](https://github.com/vrolife/modern_laptop/blob/main/drivers/redmibook_dmic/README.md)

  2. 支持 TM2113 BIOS RMARB5B0P0B0B （升级后记得关安全启动）。

- 2022年7月28日

  1. 添加键盘背光驱动。 [链接](https://github.com/vrolife/modern_laptop/blob/main/drivers/redmibook_kbd_backlight/README.md)

  2. WLAN和蓝牙驱动支持5.15内核。之前是5.17。

  3. 修复键盘背光驱动最高亮度。

  4. 为 Ubuntu 22.04 添加AMD专有驱动支持（ROCm）。[链接](https://github.com/vrolife/modern_laptop/blob/main/fixes/rocm-for-ubuntu-22.04/README.md)


- 2022年7月27日

  1. 修复不插电的情况下从休眠唤醒重启，和SSD不安全掉电。[链接](https://github.com/vrolife/modern_laptop/blob/main/fixes/resume-from-suspend-and-ssd-unsafe-shutdown/README.md)

- 2022年7月19日

  1. 添加 RTL8852BE WIFI 驱动。

- 2022年7月19日

  1. 支持从旧版升级。

- 2022年7月18日

  1. 添加蓝牙驱动。 [链接](https://github.com/vrolife/modern_laptop/blob/main/drivers/blue8852be/README.md)

- 2022年7月18日

  1. 增加指纹驱动(USB 0x10a5 0x9201)。 [链接](https://github.com/vrolife/modern_laptop/blob/main/drivers/fingerprint/README.md)

- 2022年7月12日

  1. 修复DSDT一处Buffer越界访问。

- 2022年7月10日

  1. 可选 - 将按键KP_Enter映射为Enter。[链接](https://github.com/vrolife/modern_laptop/issues/3)

  2. 修复 - 可能的，PSR导致的内核崩溃。[链接](https://github.com/vrolife/modern_laptop/blob/main/fixes/PSR-crash/README.md)

## 升级

如果之前`make install`安装过/boot/acpi_override，升级安装新版的话，会出现这样的错误

```
patching file dsdt.dsl
Reversed (or previously applied) patch detected!  Assume -R? [n] y
Hunk ##3 FAILED at 5471.
Hunk ##4 FAILED at 5479.
2 out of 4 hunks FAILED -- saving rejects to file dsdt.dsl.rej
```

这时需要启动到Grub启动菜单，在启动选项按E编辑，删除initrd后面的acpi_override部分。接着按F10启动进系统安装新版补丁。注意这时键盘是无效的，请使用外接键盘。

自动升级在`2022年7月19日星期二 下午12:39`更新后支持

## 鸣谢（无序）

- @Vliro

- @HowardZorn

- @puzzle9

- @Evan7o

- @mikethefirst0

## 更多信息

[链接](https://zhuanlan.zhihu.com/p/530643928)
