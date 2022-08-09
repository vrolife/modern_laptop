
all: acpi_override

dsdt.dat:
	/bin/sh dump_dsdt.sh

dsdt.dsl: dsdt.dat
	iasl -d dsdt.dat

dsdt.aml: dsdt.dsl
	/bin/sh ./patch.sh
	iasl dsdt.dsl

compile:
	iasl dsdt.dsl

acpi_override: dsdt.aml
	mkdir -p kernel/firmware/acpi
	cp dsdt.aml kernel/firmware/acpi/
	find kernel | cpio -H newc --create > acpi_override

install: acpi_override
	cp acpi_override /boot/acpi_override
	/bin/sh ./append-acpi_override-to-grub.sh
	rm -rf ./kernel
	rm -f dsdt* acpi_override

uninstall:
	rm /boot/acpi_override
	/bin/sh ./remove-acpi_override-from-grub.sh

test: compile install

clean:
	rm -rf ./kernel
	rm -f dsdt* acpi_override
