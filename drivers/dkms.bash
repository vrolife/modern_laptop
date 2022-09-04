#!/bin/bash
set -e
CMD="$1"
DIR="$2"

. "$DIR/dkms.conf"

mkdir -p /usr/src

if test "$CMD" = "install"; then
    if test ! -e "/usr/src/$PACKAGE_NAME-$PACKAGE_VERSION"; then
        dkms add "$DIR"
    else
        dkms remove --force $PACKAGE_NAME/$PACKAGE_VERSION
    fi

    dkms install $PACKAGE_NAME/$PACKAGE_VERSION

    if test -n "$BUILT_MODULE_NAME"; then
        modprobe -r $BUILT_MODULE_NAME
        modprobe $BUILT_MODULE_NAME
    fi
    echo "Please reboot your system."

else
    dkms remove $PACKAGE_NAME/$PACKAGE_VERSION
    if test -n "$BUILT_MODULE_NAME"; then
        modprobe -r $BUILT_MODULE_NAME
    fi
fi
