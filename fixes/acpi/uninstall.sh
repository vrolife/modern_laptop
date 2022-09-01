#!/bin/sh
set -e

rm /boot/acpi_override
sed -i 's/acpi_override//g' /etc/default/grub
/bin/sh update-grub.sh
