# SPDX-License-Identifier: GPL-2.0
#
# Makefile for the Linux Bluetooth HCI device drivers.
#

KBUILD_MODPOST_WARN=y
obj-m += btusb.o btrtl.o

all:
	make -C /lib/modules/$(KERNELRELEASE)/build M=`pwd` modules

install:
	make -C /lib/modules/$(KERNELRELEASE)/build M=`pwd` modules_install

clean:
	make -C /lib/modules/$(KERNELRELEASE)/build M=`pwd` clean
