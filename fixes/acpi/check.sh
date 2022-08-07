#!/bin/sh

if test -z "$TOP_DIR"; then
    export TOP_DIR="$(realpath "$(dirname "$(realpath $0)")/../..")"
fi

if test ! -e "$TOP_DIR/$PRODUCTION_NAME"; then
    echo "Unsupported product: $PRODUCTION_NAME"
    exit 1
fi

if test ! -e "$TOP_DIR/$PRODUCTION_NAME/$BIOS_VERSION"; then
    echo "Unsupported BIOS version: $BIOS_VERSION"
    exit 1
fi
