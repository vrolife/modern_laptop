#!/bin/sh
set -e

. /etc/os-release

if ! grep acpi_backlight /etc/default/grub >/dev/null 2>&1; then
    echo 'GRUB_CMDLINE_LINUX="$GRUB_CMDLINE_LINUX acpi_backlight=native"' >> /etc/default/grub
fi

echo Done
