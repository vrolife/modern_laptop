#!/bin/sh

if ! grep acpi_osi /etc/default/grub >/dev/null; then
    echo "GRUB_CMDLINE_LINUX=\"\$GRUB_CMDLINE_LINUX acpi_osi=! acpi_osi=Linux\"" >>/etc/default/grub
fi
