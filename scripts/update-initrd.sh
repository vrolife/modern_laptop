#!/bin/sh

if test "$ID" = "fedora" -o "$ID" = "nobara"; then
    dracut --regenerate-all
elif test "$ID" = "manjaro" -o "$ID" = "arch" -o "$ID_LIKE" = "arch"; then
    mkinitcpio -P
else
    update-initramfs -k all -u
fi
