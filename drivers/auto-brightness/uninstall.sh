#/bin/sh

if test -z "$SUDO_USER"; then
    echo "Unknown user"
    exit 1
fi

systemctl --machine=pom@.host --user stop auto-brightness

systemctl --machine=pom@.host --user disable /home/$SUDO_USER/.config/systemd/user/auto-brightness.service

runuser -u $SUDO_USER -- rm /home/$SUDO_USER/.config/systemd/user/auto-brightness.service

rm -f /opt/auto-brightness/bin/auto-brightness
