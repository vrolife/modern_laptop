#!/bin/sh

prerr() {
    printf "\e[1;31m$@\e[0m\n"
}

if test -z "$TOP_DIR"; then
    export TOP_DIR="$(realpath "$(dirname "$(realpath $0)")/../..")"
fi

if test ! -e "$TOP_DIR/$PRODUCTION_NAME"; then
    prerr "Unsupported product: $PRODUCTION_NAME"
    exit 1
fi

if test ! -e "$TOP_DIR/$PRODUCTION_NAME/$BIOS_VERSION"; then
    prerr "Unsupported BIOS version: $BIOS_VERSION"
    exit 1
fi

. /etc/os-release

if test "$ID" = "manjaro" -o "$ID_LIKE" = "arch"; then
    patch < "$TOP_DIR/${PRODUCTION_NAME}/${BIOS_VERSION}/patch.archlinux.diff"
elif test "$ID" = "opensuse-tumbleweed" -o "$ID_LIKE" = "opensuse suse"; then
    patch < "$TOP_DIR/${PRODUCTION_NAME}/${BIOS_VERSION}/patch.archlinux.diff"
else
    patch < "$TOP_DIR/${PRODUCTION_NAME}/${BIOS_VERSION}/patch.diff"
fi
