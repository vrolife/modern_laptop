#!/bin/sh

cp disable-psr.conf /etc/modprobe.d/disable-psr.conf

if test "$ID" = "fedora"; then
    dracut --regenerate-all
elif test "$ID" = "manjaro" -o "$ID_LIKE" = "arch"; then
    mkinitcpio -P
else
    update-initramfs -k all -u
fi
