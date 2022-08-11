#!/bin/sh

. /etc/os-release

if test -z "$SUDO_USER"; then
    echo "Unknown user"
    exit 1
fi

cp auto-brightness.json /etc/

mkdir -p /opt/auto-brightness/bin/

systemctl --machine=pom@.host --user stop auto-brightness

if test $ID = "manjaro" -o $ID_LIKE = "arch"; then
    cp auto-brightness.archlinux /opt/auto-brightness/bin/auto-brightness
else
    cp auto-brightness /opt/auto-brightness/bin/auto-brightness
fi

runuser -u $SUDO_USER -- mkdir -p /home/$SUDO_USER/.config/systemd/user

runuser -u $SUDO_USER -- cp auto-brightness.service /home/$SUDO_USER/.config/systemd/user/auto-brightness.service

systemctl --machine=pom@.host --user enable /home/$SUDO_USER/.config/systemd/user/auto-brightness.service

systemctl --machine=pom@.host --user start auto-brightness
