#/bin/sh

. /etc/os-release

if test $ID = "debian" -o $ID_LIKE = "debian"; then
    systemctl revert fprintd
else
    echo "see https://github.com/vrolife/modern_laptop/issues/5#issuecomment-1191811841"
    exit 1
fi
