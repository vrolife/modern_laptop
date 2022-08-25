 #!/bin/sh

 . /etc/os-release

if test "$ID" = "opensuse-tumbleweed" -o "$ID_LIKE" = "opensuse suse"; then
    update-bootloader
elif test "$ID" = "fedora"; then
    grub2-mkconfig -o /boot/grub2/grub.cfg
else
    update-grub
fi
