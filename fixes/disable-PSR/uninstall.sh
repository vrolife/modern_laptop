#!/bin/sh

rm -f /etc/modprobe.d/disable-psr.conf

update-initramfs -k all -u
