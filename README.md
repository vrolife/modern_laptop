## Enabling the WMI Module (Function Keys) on NixOS

To enable the WMI module (top function keys), you need to include the module in your `configuration.nix` and activate the corresponding service.

## 1. Import the module

Add the module to the `imports` section:

```nix
imports = [
  ./wmi.nix
];
```

## 2. Enable the WMI service

```nix
services.superduperdriverpack.wmi.enable = true;
```

## Linux Driver
-   Redmi Book Pro 15 2023 Ryzen Editions:
    Function Keys 100%
    Other: ?
-   Redmi Book Pro 14 2022 Ryzen Edition
-   Redmi Book Pro 15 2022 Ryzen Edition
-   Redmi Book Pro 15 2021 Ryzen Edition

## Redmi Book Pro 14/15 2022/2021 Ryzen Linux Driver

[English](https://github-com.translate.goog/vrolife/modern_laptop?_x_tr_sl=auto&_x_tr_tl=en&_x_tr_hl=en-US&_x_tr_pto=wapp)

## Notes

Installing dependency packages requires an Internet connection. So
please connect to the Internet in advance. Possible methods:

1.  Use a USB external network adapter recognized by Linux.
2.  Share your Android/iPhone mobile network via USB.

After updating BIOS, disable Secure Boot.

## Supported Distributions

All drivers in this repository are developed based on Ubuntu 22.04 +
kernel 5.17. Therefore, Ubuntu 22.04 is recommended.

Ubuntu 22.04 can obtain the 5.17 kernel by installing
`linux-image-oem-22.04`.

Currently Linux 6.0 performs better than 5.17 --- mainly AMDGPU does not
freeze randomly, and PSR (Panel Self Refresh) is supported (better power
saving). Ubuntu users can obtain kernel 6.0 via `linux-oem-22.04b`.

`linux-oem-6.1_6.1.0-1006.6` enables the RTL8852BE WiFi driver (LP:
#2002601).

Supported distributions:

1.  Ubuntu 22.04 LTS
2.  ArchLinux/Manjaro
3.  openSUSE Tumbleweed
4.  Fedora
5.  Linux Mint

## Dependencies

-   dkms
-   bash
-   make
-   acpica-tools
-   dmidecode
-   mokutil

## Supported Models

### TM2113 --- Redmi Book Pro 15 2022

BIOS Version: RMARB5B0P0A0A / RMARB5B0P0B0B / RMARB5B0P0C0C /
RMARB5B1P0C0C

ICC Color Profile:
[Link](https://github.com/vrolife/modern_laptop/issues/38)

Available components:

-   **blue8852be --- RTL8852BE Bluetooth driver**

-   **rtw89-lwfinger**

    Better RTL8852BE driver\
    [rtw89](https://github.com/lwfinger/rtw89)

-   **Deprecated rtl8852be --- RTL8852BE WiFi driver**

    Since my laptop has replaced rtl8852be with AX210, I can no longer
    maintain this driver. The code in the `rtl8852be_linux_6_1` branch
    compiles but is untested.

    This driver will soon be deprecated.\
    The [rtw89](https://github.com/lwfinger/rtw89) driver is a better
    choice.\
    Linux 6.1 contains code for the 8852BE WiFi driver, but you may need
    to build it manually.

    [Source](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/drivers/net/wireless/realtek/rtw89/rtw8852be.c?h=v6.1)

-   **fingerprint --- Fingerprint driver**

    Supports only USB 10a5:9201 (FPC FPC Sensor Controller).\
    [Source](https://github.com/vrolife/fingerprint-ocv)

-   **redmibook_wmi --- Function keys**

-   **redmibook_kbd_backlight --- Keyboard backlight**

-   **redmibook_dmic --- Microphone**

    Not required on Linux 5.19 or Ubuntu linux-oem-5.17.0-1014 or later;
    ACPI patch is enough.

    [Details](https://github.com/vrolife/modern_laptop/blob)

-   **disable-PSR --- Disable Panel Self Refresh**

-   **acpi --- ACPI patch**

    Fixes keyboard issues (\< Linux 5.19.11)\
    Drives microphone (\>= Ubuntu oem 5.17.0-1014, \>= mainline Linux
    5.19)\
    Fixes power management

    [Details](https://github.com/vrolife/modern_laptop/blob/main/fixes/acpi/README.md)

### TM2107 --- Redmi Book Pro 14 2022

BIOS Version: RMARB4B0P0808 / RMARB4B0P0A0A / RMARB4B0P0B0B /
RMARB4B0P0C0C

Note: TM2107 uses the same motherboard as TM2113.\
Refer to TM2113 for supported components.

### TM2019 --- RedmiBook Pro 15S

BIOS Version: RMACZ5B0P0909

Available components:

-   **acpi**

    Fix lid issue.

## Installation

``` bash
git clone https://github.com/vrolife/modern_laptop.git
cd modern_laptop
sudo /bin/sh install.sh acpi                             ## apply ACPI patch
sudo /bin/sh install.sh blue8852be                       ## 8852BE bluetooth
sudo /bin/sh install.sh rtw89-lwfinger                   ## Better RTL8852BE driver
sudo /bin/sh install.sh rtl8852be                        ## Deprecated RTL8852BE driver
sudo /bin/sh install.sh fingerprint                      ## fingerprint
sudo /bin/sh install.sh redmibook_wmi                    ## function keys
sudo /bin/sh install.sh redmibook_kbd_backlight          ## keyboard backlight
sudo /bin/sh install.sh redmibook_dmic                   ## microphone (Linux 6.x + ACPI patch may be better than this driver)
sudo /bin/sh install.sh disable-PSR                      ## disable PSR
sudo /bin/sh install.sh acpi_backlight_native            ## Fix Linux 6.1.x backlight problem.
```

## Uninstall

`sudo /bin/sh uninstall.sh component`

## Upgrade

If ACPI patch upgrade fails, see:\
https://github.com/vrolife/modern_laptop/issues/55#issuecomment-1374755208

## Known Issues

-   Current kernels (\< 6.0) do not support Yellow Carp PSR.
-   S0 suspend consumes about 3.7% battery per hour.\
    S4/S5 wake causes ACPI errors.\
    Hybrid sleep does not work.

## Changelog

(Full changelog omitted for brevity; available in original text.)
