#!/bin/bash
set -e

if test -z "$TOP_DIR"; then
    export TOP_DIR="$(realpath "$(dirname "$(realpath $0)")/..")"
fi

CMD="$1"
DIR="$2"

. "$DIR/dkms.conf"

mkdir -p /usr/src

if test "$CMD" = "install"; then
    if test -e "/usr/src/$PACKAGE_NAME-$PACKAGE_VERSION"; then
        dkms remove --all --force "$PACKAGE_NAME/$PACKAGE_VERSION" || true
        rm -rf "/usr/src/$PACKAGE_NAME-$PACKAGE_VERSION"
    fi

    dkms add "$DIR"
    dkms install $PACKAGE_NAME/$PACKAGE_VERSION

    if test -n "$BUILT_MODULE_NAME"; then
        modprobe -r $BUILT_MODULE_NAME
        modprobe $BUILT_MODULE_NAME
    fi

    /bin/sh "$TOP_DIR/scripts/update-initrd.sh"

    printf "\e[1;33mPlease reboot your system\e[0m\n"

else
    dkms remove $PACKAGE_NAME/$PACKAGE_VERSION
    if test -n "$BUILT_MODULE_NAME"; then
        modprobe -r $BUILT_MODULE_NAME
    fi
fi
