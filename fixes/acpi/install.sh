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

grub_mem_sleep() {
    if grep mem_sleep_default /etc/default/grub >/dev/null; then
        prerr "There is a mem_sleep_default parameter in /etc/default/grub!"
        prerr "You should manually add mem_sleep_default=deep to GRUB_CMDLINE_LINUX_DEFAULT if needed."
        return
    fi

    if grep 'GRUB_CMDLINE_LINUX_DEFAULT' /etc/default/grub >/dev/null; then
        sed -ire 's/GRUB_CMDLINE_LINUX_DEFAULT="(.+)"/GRUB_CMDLINE_LINUX_DEFAULT="\1 mem_sleep_default=deep"/g' /etc/default/grub
    else
        echo "GRUB_CMDLINE_LINUX_DEFAULT=\"mem_sleep_default=deep\"" >>/etc/default/grub
    fi
    
    return
}

update_grub() {
    local OVERRIDE_ACPI_OSI="$TOP_DIR/${PRODUCTION_NAME}/${BIOS_VERSION}/override_acpi_osi.sh"

    if test -e "$OVERRIDE_ACPI_OSI"; then
        sh "$OVERRIDE_ACPI_OSI"
    fi

    if grep acpi_override /etc/default/grub >/dev/null; then
        prerr "There is a acpi_override in /etc/default/grub!"
        prerr "You should you should first run uninstall.sh to remove old intallation."
        prerr "Or you can manually add mem_sleep_default=deep to GRUB_CMDLINE_LINUX_DEFAULT"
        prerr "and GRUB_EARLY_INITRD_LINUX_CUSTOM=\"acpi_override\" if needed."
        return
    fi
    
    if grep 'GRUB_EARLY_INITRD_LINUX_CUSTOM=""' /etc/default/grub >/dev/null; then
        sed -i 's/GRUB_EARLY_INITRD_LINUX_CUSTOM=""/GRUB_EARLY_INITRD_LINUX_CUSTOM="acpi_override"/g' /etc/default/grub
    else
        echo "GRUB_EARLY_INITRD_LINUX_CUSTOM=\"acpi_override\"" >>/etc/default/grub
    fi

    grub_mem_sleep

    /bin/sh "$TOP_DIR/scripts/update-grub.sh"
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

    BACKUP_DIR="$TOP_DIR/ACPI_BACKUP_$BIOS_VERSION"

    /bin/sh dump_table.sh "$BACKUP_DIR"
    iasl -e `ls "$BACKUP_DIR"/ssdt*.dat|sort -V -r` -p "$(pwd)/dsdt" -d "$BACKUP_DIR/dsdt.dat"

    cp dsdt.dsl dsdt.dsl.origin

    if ! patch < "$PATCH"; then
        prerr "Patch ACPI table failed!"
        exit 1
    fi

    iasl -ve dsdt.dsl
    cp dsdt.aml kernel/firmware/acpi/
}

if test -z "$TOP_DIR"; then
    export TOP_DIR="$(realpath "$(dirname "$(realpath $0)")/../..")"
fi

if test -n "$TEST_ACPI_TM2113"; then
    export PRODUCTION_NAME=TM2113-Redmi_Book_Pro_15_2022
    export BIOS_VERSION=RMARB5B0P0B0B
    export TEST_ACPI=1
fi

if test -n "$TEST_ACPI_TM2019"; then
    export PRODUCTION_NAME=TM2019-RedmiBook_Pro_15S
    export BIOS_VERSION=RMACZ5B0P0909
    export TEST_ACPI=1
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

if test -z "$TEST_ACPI"; then
    cp acpi_override /boot/acpi_override

    if test -z "$NO_GRUB"; then
        update_grub
    fi
fi

if test -z "$ACPI_KEEP_FILES"; then
    clean
fi

printf "\e[1;33mSuccessful!!!\e[0m\n"
printf "\e[1;33mPlease keep this folder '$BACKUP_DIR' for future upgrades\e[0m\n"
