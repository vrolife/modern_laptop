#/bin/sh

if test -z "$TOP_DIR"; then
    export TOP_DIR="$(realpath "$(dirname "$(realpath $0)")/../..")"
fi

if test -e "$TOP_DIR/original_dsdt.dat"; then
    echo "Upgrading"
    cp "$TOP_DIR/original_dsdt.dat" dsdt.dat
else
    echo "Please keep this file '$TOP_DIR/original_dsdt.dat' for upgrading"
    cp /sys/firmware/acpi/tables/DSDT dsdt.dat
    cp dsdt.dat "$TOP_DIR/original_dsdt.dat"
fi
