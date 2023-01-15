
## Linux 驱动 / Linux Driver

- Redmi Book Pro 14 2022 Ryzen 锐龙版

- Redmi Book Pro 15 2022 Ryzen 锐龙版

- Redmi Book Pro 15 2021 Ryzen 锐龙版

## Redmi Book Pro 14/15 2022/2021 Ryzen Linux Driver

[English](https://github-com.translate.goog/vrolife/modern_laptop?_x_tr_sl=auto&_x_tr_tl=en&_x_tr_hl=en-US&_x_tr_pto=wapp)

## 提示

安装依赖包需要网络连接。所以请提前接入互联网。可能的办法有：

1. 使用Linux能识别的USB外置网卡。

2. 通过USB共享你的Android/iPhone的移动网络.

更新BIOS后 关闭安全启动

## 发行版

此仓库中的全部驱动都是基于Ubuntu 22.04 + 5.17 内核开发。所以，建议使用Ubuntu 22.04。

Ubuntu 22.04 可通过安装包linux-image-oem-22.04获得5.17内核。

目前Linux 6.0表现比5.17更好，主要是AMDGPU不会随机卡死，并且支持PSR功能（更省电）。Ubuntu用户可通过安装`linux-oem-22.04b`获得6.0内核

支持的发行版：

1. Ubuntu 22.04 LTS

2. ArchLinux/Manjaro

3. openSUSE Tumbleweed

4. Fedora

5. Linux Mint

## 依赖包

- dkms
- bash
- make
- acpica-tools
- dmidecode
- mokutil

## 支持机型

- TM2113-Redmi_Book_Pro_15_2022

    BIOS Version: RMARB5B0P0A0A RMARB5B0P0B0B RMARB5B0P0C0C RMARB5B1P0C0C

    校色文件 ICC Profile: [Link](https://github.com/vrolife/modern_laptop/issues/38)

    可用组件:

    * blue8852be **RTL8852BE 蓝牙驱动**

    * Deprecated rtl8852be  **RTL8852BE 网卡驱动**

        此驱动已经放弃。Linux 6.1 有内建的8852BE wifi驱动，请考虑安装Linux 6.1

        [源码](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/drivers/net/wireless/realtek/rtw89/rtw8852be.c?h=v6.1)

    * fingerprint **指纹驱动**

        仅支持 USB 10a5:9201(FPC FPC Sensor Controller).

        [源码](https://github.com/vrolife/fingerprint-ocv)

    * redmibook_wmi **功能键**

    * redmibook_kbd_backlight **键盘背光**

    * redmibook_dmic **麦克风**

        Linux 5.19 或 linux-oem-5.17.0-1014 之后不需要要此驱动，安装ACPI补丁即可。

        [详情](https://github.com/vrolife/modern_laptop/blob

    * disable-PSR **关闭PSR(屏幕面板自刷新 Panel Self Refresh)**

    * acpi **ACPI补丁 ACPI Patch**

      修复键盘问题(Linux<5.19.11)

      驱动麦克风(>=Ubuntu Linux oem 5.17.0-1014, >=Mainline Linux 5.19)

      修复电源管理

      [详情](https://github.com/vrolife/modern_laptop/blob/main/fixes/acpi/README.md)


- TM2107-Redmi_Book_Pro_14_2022

    BIOS Version: RMARB4B0P0808 RMARB4B0P0A0A RMARB4B0P0B0B RMARB4B0P0C0C

    注意: TM2107 用的是和 TM2113 一样的主板. 支持的组件请参考 TM2113

- TM2019-RedmiBook_Pro_15S

    BIOS Version: RMACZ5B0P0909

    可用组件:

    * acpi

      修复笔记本合盖问题。 Fix lid

## 安装

```bash
git clone https://github.com/vrolife/modern_laptop.git
cd modern_laptop
sudo /bin/sh install.sh acpi                             ## apply ACPI patch
sudo /bin/sh install.sh blue8852be                       ## 8852BE blutooth
sudo echo Deprecated ！！！ /bin/sh install.sh rtl8852be  ## 8852BE wifi (Linux 6.1 有内建的8852BE wifi驱动，请考虑安装Linux 6.1)
sudo /bin/sh install.sh fingerprint                      ## fingerprint
sudo /bin/sh install.sh redmibook_wmi                    ## function keys
sudo /bin/sh install.sh redmibook_kbd_backlight          ## keyboard backlight
sudo /bin/sh install.sh redmibook_dmic                   ## microphone
sudo /bin/sh install.sh disable-PSR                      ## disable PSR
```

## 卸载

`sudo /bin/sh uninstall.sh component`

## 升级

如果升级ACPI补丁失败，请看[此处](https://github.com/vrolife/modern_laptop/issues/55#issuecomment-1374755208)

## 已知问题

- 目前内核（<6.0）不支持 Yellow Carp PSR。 [链接](https://lore.kernel.org/all/20220510204508.506089-15-dingchen.zhang@amd.com/T/)

- S0休眠耗电，大约3.7%每小时。S4/S5 休眠唤醒ACPI报错。混合休眠不工作。

## 变更

- 2023/1/15

  1. Patching support for Linux Mint. [Link](https://github.com/vrolife/modern_laptop/issues/58)

- 2023/1/9

  1. Update README.md

  2. Linux 6.0 recommended, AMDGPU no random freezing, PSR available (save power)

- 2022/11/5

  1. ACPI patch for TM2113 RMARB5B1P0C0C

- 2022/11/1

  1. ACPI patch for TM2019-RedmiBook_Pro_15S (RedmiBook Pro 2021) - Fix lid issue.

- 2022/10/31

  1. Fingerprint driver for Fedora/Nobara

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


## 鸣谢（无序）

- @Vliro

- @HowardZorn

- @puzzle9

- @Evan7o

- @mikethefirst0

## 更多信息

[链接](https://zhuanlan.zhihu.com/p/530643928)
