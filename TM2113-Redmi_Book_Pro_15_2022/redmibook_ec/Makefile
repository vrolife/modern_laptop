KBUILD_MODPOST_WARN=y
obj-m += redmibook_ec.o

all:
	make -C /lib/modules/$(KERNELRELEASE)/build M=`pwd` modules
	
install:
	make -C /lib/modules/$(KERNELRELEASE)/build M=`pwd` modules_install

clean:
	make -C /lib/modules/$(KERNELRELEASE)/build M=`pwd` clean
