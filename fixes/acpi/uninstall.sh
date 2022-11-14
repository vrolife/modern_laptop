#!/bin/sh
set -e

if test -z "$TOP_DIR"; then
    export TOP_DIR="$(realpath "$(dirname "$(realpath $0)")/../..")"
fi

rm /boot/acpi_override
sed -i 's/acpi_override//g' /etc/default/grub
sed -i 's/mem_sleep_default=deep//g' /etc/default/grub

/bin/sh "$TOP_DIR/scripts/update-grub.sh"
