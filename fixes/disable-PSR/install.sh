#!/bin/sh

cp disable-psr.conf /etc/modprobe.d/disable-psr.conf

if test "$ID" = "fedora"; then
    dracut --regenerate-all
else
    update-initramfs -k all -u
fi
