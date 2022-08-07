#!/bin/sh

sed -i 's/acpi_override//g' /etc/default/grub
update-grub
