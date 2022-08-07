#!/bin/sh
set -e

export TOP_DIR="$(dirname $(realpath $0))"

COMPONENT="$1"
COMMAND="$(basename "$0")"

. /etc/os-release

if test $(id -u) -ne 0; then
    echo "Permission denied"
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

if test "$(iasl -v|grep version|cut -d ' ' -f 5 -)" != "20200925"; then
    echo "Please install acpica-tools 20200925"
    exit 1
fi

export PRODUCTION_NAME="$(dmidecode -s baseboard-product-name)-$(dmidecode -s system-product-name|sed 's/ /_/g')"
export BIOS_VERSION="$(dmidecode -s bios-version)"

if test -z "$COMPONENT"; then
    echo "Usage:"
    echo "sudo /bin/sh ./install.sh acpi # apply keyboard patch"
    echo "sudo /bin/sh ./install.sh redmibook_dmic # apply microphone patch"
    exit 0
fi

install_driver() {
    DIR="$1"
    if test "$COMMAND" = "install.sh"; then
        CMD=install
    else
        CMD=uninstall
    fi

    if test -e "$DIR/$CMD.sh"; then
        (cd "$DIR"; /bin/sh "$DIR/$CMD.sh")
    elif test -e "$DIR/dkms.conf"; then
        env bash "$TOP_DIR/drivers/dkms.bash" $CMD "$DIR"
    elif test -e "$DIR/Makefile"; then
        make -C "$DIR" TOP_DIR="$TOP_DIR" $CMD
    fi
}

if test -e "$TOP_DIR/drivers/$COMPONENT"; then
    install_driver "$TOP_DIR/drivers/$COMPONENT"

elif test -e "$TOP_DIR/fixes/$COMPONENT"; then
    install_driver "$TOP_DIR/fixes/$COMPONENT"
fi
