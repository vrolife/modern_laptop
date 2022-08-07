#!/bin/bash
set -x
CMD="$1"
DIR="$2"

. "$DIR/dkms.conf"

mkdir -p /usr/src

if test "$CMD" = "install"; then
    if test ! -e "/usr/src/$PACKAGE_NAME-$PACKAGE_VERSION"; then
        dkms add "$DIR"
    fi

    dkms install $PACKAGE_NAME/$PACKAGE_VERSION

    if test -n "$BUILT_MODULE_NAME"; then
        modprobe -r $BUILT_MODULE_NAME
        modprobe $BUILT_MODULE_NAME
    fi
else
    dkms remove $PACKAGE_NAME/$PACKAGE_VERSION
    if test -n "$BUILT_MODULE_NAME"; then
        modprobe -r $BUILT_MODULE_NAME
    fi
fi
