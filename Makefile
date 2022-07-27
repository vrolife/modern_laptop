
all: acpi_override

dsdt.dat:
	/bin/sh dump_dsdt.sh

dsdt.dsl: dsdt.dat
	iasl -d dsdt.dat

dsdt.aml: dsdt.dsl .bios/patch.diff
	patch < .bios/patch.diff
	iasl dsdt.dsl

compile:
	iasl dsdt.dsl

acpi_override: dsdt.aml
	mkdir -p kernel/firmware/acpi
	cp dsdt.aml kernel/firmware/acpi/
	find kernel | cpio -H newc --create > acpi_override

install: acpi_override
	cp acpi_override /boot/acpi_override

test: compile install

grub:
	echo "GRUB_EARLY_INITRD_LINUX_CUSTOM=\"acpi_override\"" >>/etc/default/grub
	update-grub

redmibook_wmi:
	mkdir -p /usr/src
	-dkms add .product/redmibook_wmi
	dkms install redmibook_wmi/1.0.2
	-modprobe -r redmibook_wmi
	modprobe redmibook_wmi

blue8852be:
	mkdir -p /usr/src
	-dkms add .product/blue8852be
	dkms install blue8852be/1.0.1
	-modprobe -r btusb btrtl
	modprobe btusb btrtl

rtl8852be:
	mkdir -p /usr/src
	-dkms add .product/rtl8852be
	dkms install rtl8852be/1.0.0
	-modprobe -r 8852be
	modprobe 8852be

clean:
	rm -rf ./kernel
	rm -f dsdt* acpi_override
	rm -f .bios .product
