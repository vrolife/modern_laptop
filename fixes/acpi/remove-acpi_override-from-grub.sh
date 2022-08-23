#!/bin/sh

. /etc/os-release

sed -i 's/acpi_override//g' /etc/default/grub

if test "$ID" = "opensuse-tumbleweed" -o "$ID_LIKE" = "opensuse suse"; then
    update-bootloader
else
    update-grub
fi
