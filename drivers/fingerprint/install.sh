#/bin/sh

prerr() {
    printf "\e[1;31m$@\e[0m\n"
}

. /etc/os-release

if test "$ID" = "debian" -o "$ID_LIKE" = "debian"; then
    apt-get install -y \
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

elif test "$ID" = "manjaro" -o "$ID_LIKE" = "arch"; then
    pacman -S libusb libevent libdbus openssl opencv fprintd

    systemctl stop fprintd

    mkdir -p /opt/fingerpp/bin/
    cp fingerpp3.archlinux /opt/fingerpp/bin/fingerpp
    chmod +x /opt/fingerpp/bin/fingerpp

    export EDITOR="$(pwd)/edit.sh"
    chmod +x "$EDITOR"

    systemctl edit fprintd
    systemctl restart fprintd

elif test "$ID" = "opensuse-tumbleweed" -o "$ID_LIKE" = "opensuse suse"; then

    zypper install libusb-1_0-devel libevent-devel libdbus-1-3 openssl opencv fprintd

    systemctl stop fprintd

    mkdir -p /opt/fingerpp/bin/
    cp fingerpp3.archlinux /opt/fingerpp/bin/fingerpp
    chmod +x /opt/fingerpp/bin/fingerpp

    export EDITOR="$(pwd)/edit.sh"
    chmod +x "$EDITOR"

    systemctl edit fprintd
    systemctl restart fprintd

elif test "$ID" = "fedora"; then

    prerr "unsupported"
    exit 1

else
    prerr "Unsupported environment."
    exit 1
fi
