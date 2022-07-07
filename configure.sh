#!/bin/sh

. /etc/os-release

if test $ID = "debian" -o $ID_LIKE = "debian"; then
    apt install make acpica-tools dmidecode
elif test $ID = "manjaro" -o $ID_LIKE = "arch"; then
    pacman -S make acpica dmidecode cpio
else
    echo "Unknown environment"
    exit -1
fi

PRODUCTION_NAME="$(dmidecode -s baseboard-product-name)-$(dmidecode -s system-product-name|sed 's/ /_/g')"
BIOS_VERSION="$(dmidecode -s bios-version)"

if test ! -e "$PRODUCTION_NAME"; then
    echo "Unsupported product"
fi

ln -s "$PRODUCTION_NAME" .product
ln -s "$PRODUCTION_NAME/$BIOS_VERSION" .bios

echo "run \`make install\` to build and install acpi_override"
echo "run \`make grub\` to load acpi_override at boot"
