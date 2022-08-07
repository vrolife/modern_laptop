#!/bin/sh

cp disable-psr.conf /etc/modprobe.d/disable-psr.conf

update-initramfs -k all -u
