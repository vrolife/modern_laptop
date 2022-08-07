#/bin/sh

. /etc/os-release

if test $ID = "debian" -o $ID_LIKE = "debian"; then
    apt update && apt-get install -y \
        libssl3 \
        libopencv-dev \
        libusb-1.0-0 \
        fprintd \
        libevent-core-2.1-7 \
        libdbus-1-3

    systemctl stop fprintd

    cp fingerpp2 /opt/fingerpp/bin/fingerpp
    chmod +x /opt/fingerpp/bin/fingerpp

    export EDITOR="$(pwd)/edit.sh"
    chmod +x "$EDITOR"

    systemctl edit fprintd
    systemctl restart fprintd
else
    echo "see https://github.com/vrolife/modern_laptop/issues/5#issuecomment-1191811841"
    exit 1
fi
