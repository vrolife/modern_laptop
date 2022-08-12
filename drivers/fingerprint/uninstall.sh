#/bin/sh

prerr() {
    echo "\e[1;31m$@\e[0m"
}

. /etc/os-release

if test $ID = "debian" -o $ID_LIKE = "debian"; then
    systemctl revert fprintd
else
    prerr "see https://github.com/vrolife/modern_laptop/issues/5#issuecomment-1191811841"
    exit 1
fi
