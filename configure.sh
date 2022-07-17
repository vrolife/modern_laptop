#!/bin/sh

. /etc/os-release

if test $(id -u) -ne 0; then
    echo "usage: sudo /bin/sh configure.sh"
    exit 1
fi

if test $ID = "debian" -o $ID_LIKE = "debian"; then
    apt install make acpica-tools dmidecode
elif test $ID = "manjaro" -o $ID_LIKE = "arch"; then
    pacman -S make acpica dmidecode cpio
else
    echo "Unknown environment"
    exit 1
fi

PRODUCTION_NAME="$(dmidecode -s baseboard-product-name)-$(dmidecode -s system-product-name|sed 's/ /_/g')"
BIOS_VERSION="$(dmidecode -s bios-version)"

if test ! -e "$PRODUCTION_NAME"; then
    echo "Unsupported product"
    exit 1
fi

if test ! -e "$PRODUCTION_NAME/$BIOS_VERSION"; then
    echo "Unsupported BIOS version"
    exit 1
fi

ln -s "$PRODUCTION_NAME" .product
ln -s "$PRODUCTION_NAME/$BIOS_VERSION" .bios

echo "run \`sudo make install\` to build and install acpi_override"
echo "run \`sudo make grub\` to load acpi_override at boot"
