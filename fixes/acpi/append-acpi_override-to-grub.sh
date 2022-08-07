#!/bin/sh

if ! grep acpi_osi /etc/default/grub >/dev/null; then
    echo "GRUB_CMDLINE_LINUX=\"\$GRUB_CMDLINE_LINUX acpi_osi=! acpi_osi=Linux\"" >>/etc/default/grub
fi

if grep acpi_override /etc/default/grub >/dev/null; then
    exit 0
fi

if grep 'GRUB_EARLY_INITRD_LINUX_CUSTOM=""' /etc/default/grub >/dev/null; then
    sed -i 's/GRUB_EARLY_INITRD_LINUX_CUSTOM=""/GRUB_EARLY_INITRD_LINUX_CUSTOM="acpi_override"/g' /etc/default/grub
else
    echo "GRUB_EARLY_INITRD_LINUX_CUSTOM=\"acpi_override\"" >>/etc/default/grub
fi

update-grub
