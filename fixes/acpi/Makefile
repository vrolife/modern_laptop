
all: acpi_override

dsdt.dat:
	/bin/sh dump_dsdt.sh

dsdt.dsl: dsdt.dat
	iasl -d dsdt.dat

dsdt.aml: dsdt.dsl
	/bin/sh ./patch.sh
	iasl -ve dsdt.dsl

compile:
	iasl -ve dsdt.dsl

acpi_override: dsdt.aml
	mkdir -p kernel/firmware/acpi
	cp dsdt.aml kernel/firmware/acpi/
	find kernel | cpio -H newc --create > acpi_override

install: acpi_override
	cp acpi_override /boot/acpi_override
	/bin/sh ./append-acpi_override-to-grub.sh
	rm -rf ./kernel
	rm -f dsdt* acpi_override
	@echo "\e[1;33mPlease keep this file '${TOP_DIR}/original_dsdt.dat' for future upgrade\e[0m"

uninstall:
	rm /boot/acpi_override
	/bin/sh ./remove-acpi_override-from-grub.sh

test: compile install

clean:
	rm -rf ./kernel
	rm -f dsdt* acpi_override
