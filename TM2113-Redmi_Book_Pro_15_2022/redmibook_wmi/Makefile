KVER ?= `uname -r`
KBUILD_MODPOST_WARN=y
obj-m += redmibook_wmi.o

all:
	make -C /lib/modules/$(KVER)/build M=`pwd` modules
	
install:
	make -C /lib/modules/$(KVER)/build M=`pwd` modules_install

clean:
	make -C /lib/modules/$(KVER)/build M=`pwd` clean
