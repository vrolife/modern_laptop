# SPDX-License-Identifier: GPL-2.0+
# Yellow Carp platform Support

snd-soc-acp6x-mach-objs := acp6x-mach.o

obj-m   += snd-soc-acp6x-mach.o

all:
	make -C /lib/modules/$(KERNELRELEASE)/build M=`pwd` modules

install:
	make -C /lib/modules/$(KERNELRELEASE)/build M=`pwd` modules_install

clean:
	make -C /lib/modules/$(KERNELRELEASE)/build M=`pwd` clean
