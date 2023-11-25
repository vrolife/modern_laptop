#!/bin/sh
set -e

if test -z "$TOP_DIR"; then
    export TOP_DIR="$(realpath "$(dirname "$(realpath $0)")/../..")"
fi

rm -f /boot/acpi_override
sed -i 's/acpi_override//g' /etc/default/grub

if test "$(dmidecode -s baseboard-product-name)" = "TM2019" ; then
    sed -i 's/mem_sleep_default=deep//g' /etc/default/grub
fi

/bin/sh "$TOP_DIR/scripts/update-grub.sh"
