#!/bin/sh

. /etc/os-release

sed -i 's/acpi_override//g' /etc/default/grub

/bin/sh update-grub.sh
