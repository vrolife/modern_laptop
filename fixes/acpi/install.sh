#!/bin/sh
set -e

. /etc/os-release

prerr() {
    printf "\e[1;31m$@\e[0m\n"
}

clean() {
    rm -rf ./kernel
    rm -f dsdt* ssdt* acpi_override
}

update_grub() {
    if ! grep acpi_osi /etc/default/grub >/dev/null; then
        echo "GRUB_CMDLINE_LINUX=\"\$GRUB_CMDLINE_LINUX acpi_osi=! acpi_osi=Linux\"" >>/etc/default/grub
    fi

    if grep acpi_override /etc/default/grub >/dev/null; then
        return
    fi

    if grep 'GRUB_EARLY_INITRD_LINUX_CUSTOM=""' /etc/default/grub >/dev/null; then
        sed -i 's/GRUB_EARLY_INITRD_LINUX_CUSTOM=""/GRUB_EARLY_INITRD_LINUX_CUSTOM="acpi_override"/g' /etc/default/grub
    else
        echo "GRUB_EARLY_INITRD_LINUX_CUSTOM=\"acpi_override\"" >>/etc/default/grub
    fi

    /bin/sh ./update-grub.sh
}

patch_dsdt() {
    if test "$ID" = "manjaro" -o "$ID" = "arch" -o "$ID_LIKE" = "arch"; then
        local PATCH="$TOP_DIR/${PRODUCTION_NAME}/${BIOS_VERSION}/patch.archlinux.diff"
    elif test "$ID" = "opensuse-tumbleweed" -o "$ID_LIKE" = "opensuse suse"; then
        local PATCH="$TOP_DIR/${PRODUCTION_NAME}/${BIOS_VERSION}/patch.archlinux.diff"
    else
        local PATCH="$TOP_DIR/${PRODUCTION_NAME}/${BIOS_VERSION}/patch.diff"
    fi

    if test ! -e "$PATCH"; then
        return
    fi

    /bin/sh dump_table.sh dsdt
    iasl -d dsdt.dat

    patch < "$PATCH"

    iasl -ve dsdt.dsl
    cp dsdt.aml kernel/firmware/acpi/
}

# not working, do not call this function
patch_dmic() {
    local KO="/lib/modules/$(uname -r)/kernel/sound/soc/amd/yc/snd-soc-acp6x-mach.ko"
    local PATCH="$TOP_DIR/${PRODUCTION_NAME}/${BIOS_VERSION}/ssdt21.diff"

    if test -e "/lib/modules/$(uname -r)/updates/dkms/snd-soc-acp6x-mach.ko"; then
        return
    fi

    if test ! -e "$KO" -o ! -e "$PATCH"; then
        return
    fi

    if !grep AcpDmicConnected "$KO" >/dev/null 2>&1; then
        return
    fi

    /bin/sh dump_table.sh ssdt21
    iasl -d ssdt21.dat

    patch < "$PATCH"

    iasl -ve ssdt21.dsl
    cp ssdt21.aml kernel/firmware/acpi/
}

if test -z "$TOP_DIR"; then
    export TOP_DIR="$(realpath "$(dirname "$(realpath $0)")/../..")"
fi

if test -n "$TEST_ACPI"; then
    export PRODUCTION_NAME=TM2113-Redmi_Book_Pro_15_2022
    export BIOS_VERSION=RMARB5B0P0B0B
fi

if test ! -e "$TOP_DIR/$PRODUCTION_NAME"; then
    prerr "Unsupported product: $PRODUCTION_NAME"
    exit 1
fi

if test ! -e "$TOP_DIR/$PRODUCTION_NAME/$BIOS_VERSION"; then
    prerr "Unsupported BIOS version: $BIOS_VERSION"
    exit 1
fi

clean

mkdir -p kernel/firmware/acpi

patch_dsdt

find kernel | cpio -H newc --create > acpi_override
cp acpi_override /boot/acpi_override

if test -z "$NO_GRUB"; then
    update_grub
fi

clean
