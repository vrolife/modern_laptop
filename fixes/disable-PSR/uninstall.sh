#!/bin/sh
set -e

if test -z "$TOP_DIR"; then
    export TOP_DIR="$(realpath "$(dirname "$(realpath $0)")/../..")"
fi

rm -f /etc/modprobe.d/disable-psr.conf

/bin/sh "$TOP_DIR/scripts/update-initrd.sh"
