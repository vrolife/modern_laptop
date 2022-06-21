
all: acpi_override

dsdt.dat:
	cat /sys/firmware/acpi/tables/DSDT > dsdt.dat

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

check:
	cp /sys/firmware/acpi/tables/DSDT dsdt_running.dat
	iasl -d dsdt_running.dat
	cat dsdt_running.dsl|grep -C 20 -i KEY

install: acpi_override
	cp acpi_override /boot/acpi_override

grub:
	echo "GRUB_EARLY_INITRD_LINUX_CUSTOM=\"acpi_override\"" >>/etc/default/grub
	update-grub2

redmibook_wmi:
	mkdir -p /usr/src
	dkms add .product/redmibook_wmi
	dkms install redmibook_wmi/1.0.1
	modprobe redmibook_wmi

clean:
	rm -rf ./kernel
	rm -f dsdt* acpi_override
	rm -f .bios .product
