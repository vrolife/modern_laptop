#!/bin/sh
set -e

prerr() {
    printf "\e[1;31m$@\e[0m\n"
}

prerr 'Please manually remove GRUB_CMDLINE_LINUX="$GRUB_CMDLINE_LINUX acpi_backlight=native" from /etc/default/grub'

exit 1
