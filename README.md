# Redmi Book Pro 14/15 2022 锐龙版 Linux 驱动

# Redmi Book Pro 14/15 2022 Ryzen Linux Driver

[English](https://github-com.translate.goog/vrolife/modern_laptop?_x_tr_sl=auto&_x_tr_tl=en&_x_tr_hl=en-US&_x_tr_pto=wapp)

# 依赖

- bash
- make
- acpica-tools (20200925)
- dmidecode

# 组件

Drivers:

- redmibook_wmi
- redmibook_kbd_backlight
- redmibook_dmic
- fingerprint
- blue8852be
- rtl8852be

Fixes:

- acpi（修复键盘和电源管理，强烈推荐！）
- disable-PSR

# 安装

```
git clone https://github.com/vrolife/modern_laptop.git
cd modern_laptop
sudo /bin/sh install.sh acpi        # keyboard ACPI patch
sudo /bin/sh install.sh blue8852be  # 8852be blutooth
sudo /bin/sh install.sh rtl8852be   # 8852be wifi
sudo /bin/sh install.sh fingerprint # fingerprint
sudo /bin/sh install.sh redmibook_* # redmibook drivers. see `组件`
sudo /bin/sh install.sh disable-PSR # disable PSR
```

安装后，目录下的original_dsdt.dat是原版DSDT,请保留此文件用于将来升级DSDT补丁。

# 卸载

`sudo /bin/sh uninstall.sh driver_name`

# 支持机型

- TM2113-Redmi_Book_Pro_15_2022

    BIOS Version: RMARB5B0P0A0A RMARB5B0P0B0B

- TM2107-Redmi_Book_Pro_14_2022

    BIOS Version: RMARB4B0P0808

# 已知问题

- 目前内核（<=5.19）不支持 Yellow Carp PSR。 [传送](https://lore.kernel.org/all/20220510204508.506089-15-dingchen.zhang@amd.com/T/)

- S0休眠耗电，大约3.7%每小时。S4/S5 休眠唤醒ACPI报错。混合休眠不工作。

# 变更

- 2022年8月7日

  1. 改进安装方式

  2. 指纹新增fingerpp2.archlinux。[传送](https://github.com/vrolife/modern_laptop/blob/main/drivers/fingerprint/README.archlinux.md)

  3. redmibook_dmic支持TM2107

  4. redmibook_kbd_backlight支持自动加载。不必写入/etc/modules

- 2022年8月1日

  1. 新增麦克风驱动（Redmibook Pro 15 2022 & Linux 5.17）。[传送](https://github.com/vrolife/modern_laptop/blob/main/drivers/redmibook_dmic/README.md)

  2. 支持 TM2113 BIOS RMARB5B0P0B0B （升级后记得关安全启动）。

- 2022年7月28日

  1. 添加键盘背光驱动。 [传送](https://github.com/vrolife/modern_laptop/blob/main/drivers/redmibook_kbd_backlight/README.md)

  2. WLAN和蓝牙驱动支持5.15内核。之前是5.17。

  3. 修复键盘背光驱动最高亮度。

  4. 为 Ubuntu 22.04 添加AMD专有驱动支持（ROCm）。[传送](https://github.com/vrolife/modern_laptop/blob/main/fixes/rocm-for-ubuntu-22.04/README.md)


- 2022年7月27日

  1. 修复不插电的情况下从休眠唤醒重启，和SSD不安全掉电。[传送](https://github.com/vrolife/modern_laptop/blob/main/fixes/resume-from-suspend-and-ssd-unsafe-shutdown/README.md)

- 2022年7月19日

  1. 添加 RTL8852BE WIFI 驱动。

- 2022年7月19日

  1. 支持从旧版升级。

- 2022年7月18日

  1. 添加蓝牙驱动。 [传送](https://github.com/vrolife/modern_laptop/blob/main/drivers/blue8852be/README.md)

- 2022年7月18日

  1. 增加指纹驱动(USB 0x10a5 0x9201)。 [传送](https://github.com/vrolife/modern_laptop/blob/main/drivers/fingerprint/README.md)

- 2022年7月12日

  1. 修复DSDT一处Buffer越界访问。

- 2022年7月10日

  1. 可选 - 将按键KP_Enter映射为Enter。[传送](https://github.com/vrolife/modern_laptop/issues/3)

  2. 修复 - 可能的，PSR导致的内核崩溃。[传送](https://github.com/vrolife/modern_laptop/blob/main/fixes/PSR-crash/README.md)

# 升级

如果之前`make install`安装过/boot/acpi_override，升级安装新版的话，会出现这样的错误

```
patching file dsdt.dsl
Reversed (or previously applied) patch detected!  Assume -R? [n] y
Hunk #3 FAILED at 5471.
Hunk #4 FAILED at 5479.
2 out of 4 hunks FAILED -- saving rejects to file dsdt.dsl.rej
```

这时需要启动到Grub启动菜单，在启动选项按E编辑，删除initrd后面的acpi_override部分。接着按F10启动进系统安装新版补丁。注意这时键盘是无效的，请使用外接键盘。

自动升级在`2022年7月19日星期二 下午12:39`更新后支持

# More information

[更多信息](https://zhuanlan.zhihu.com/p/530643928)
