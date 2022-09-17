#/bin/sh

prerr() {
    printf "\e[1;31m$@\e[0m\n"
}

. /etc/os-release

if test -z "$SUDO_USER"; then
    prerr "Unknown user"
    exit 1
fi

systemctl --machine=$SUDO_USER@.host --user stop auto-brightness

systemctl --machine=$SUDO_USER@.host --user disable /home/$SUDO_USER/.config/systemd/user/auto-brightness.service

runuser -u $SUDO_USER -- rm /home/$SUDO_USER/.config/systemd/user/auto-brightness.service

rm -f /opt/auto-brightness/bin/auto-brightness
