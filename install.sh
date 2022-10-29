#!/bin/sh
set -e

export TOP_DIR="$(dirname $(realpath $0))"

COMPONENT="$1"
COMMAND="$(basename "$0")"

if test -z "$COMPONENT" -o "$COMPONENT" = "-h" -o "$COMPONENT" = "--help"; then
    echo "Usage:"
    echo "sudo /bin/sh ./install.sh acpi # apply keyboard patch"
    echo "sudo /bin/sh ./install.sh redmibook_dmic # apply microphone patch"
    echo "Force install:"
    echo "sudo FORCE_INSTALL=1 /bin/sh ./install.sh redmibook_dmic"
    exit 0
fi

prerr() {
    printf "\e[1;31m$@\e[0m\n"
}

. /etc/os-release

if test $(id -u) -ne 0; then
    prerr "Permission denied"
    exit 1
fi

if test ! -e ".skip_update"; then
    if test "$ID" = "debian" -o "$ID_LIKE" = "debian"; then
        apt update
        apt install -y dkms bash make acpica-tools dmidecode mokutil patch diffutils

    elif test "$ID" = "manjaro" -o "$ID" = "arch" -o "$ID_LIKE" = "arch"; then
        pacman -Syyu
        pacman -S dkms bash make acpica dmidecode mokutil patch diffutils cpio

    elif test "$ID" = "opensuse-tumbleweed" -o "$ID_LIKE" = "opensuse suse"; then
        zypper refresh
        zypper install dkms bash make acpica dmidecode mokutil patch diffutils

    elif test "$ID" = "fedora" -o "$ID" = "nobara"; then
        dnf install dkms bash make acpica-tools dmidecode mokutil patch diffutils dracut

    else
        prerr "Unknown environment"
        exit 1
    fi
fi

if test "$(mokutil --sb-state | grep "SecureBoot disabled")" != "SecureBoot disabled" -a  -z "$FORCE_INSTALL"; then
    prerr "SecureBoot enabled!. ACPI patch and dkms driver may not work!"
    prerr "Force install see \`sudo /bin/sh install.sh --help\`"
    exit 1
fi

export PRODUCTION_NAME="$(dmidecode -s baseboard-product-name)-$(dmidecode -s system-product-name|sed 's/ /_/g')"
export BIOS_VERSION="$(dmidecode -s bios-version)"

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
else
    prerr "Component $COMPONENT does not exists"
fi
