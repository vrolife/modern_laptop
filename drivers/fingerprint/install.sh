#/bin/sh

prerr() {
    printf "\e[1;31m$@\e[0m\n"
}

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

    mkdir -p /opt/fingerpp/bin/
    cp fingerpp3 /opt/fingerpp/bin/fingerpp
    chmod +x /opt/fingerpp/bin/fingerpp

    export EDITOR="$(pwd)/edit.sh"
    chmod +x "$EDITOR"

    systemctl edit fprintd
    systemctl restart fprintd

elif test $ID = "manjaro" -o $ID_LIKE = "arch"; then
    pacman -Syyu
    pacman -S libusb libevent libdbus openssl opencv fprintd

    systemctl stop fprintd

    mkdir -p /opt/fingerpp/bin/
    cp fingerpp3.archlinux /opt/fingerpp/bin/fingerpp
    chmod +x /opt/fingerpp/bin/fingerpp

    export EDITOR="$(pwd)/edit.sh"
    chmod +x "$EDITOR"

    systemctl edit fprintd
    systemctl restart fprintd

else
    prerr "Unsupported environment. see https://github.com/vrolife/modern_laptop/issues/5#issuecomment-1191811841"
    exit 1
fi
