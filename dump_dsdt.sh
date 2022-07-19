#/bin/sh

if test -e "original_dsdt.dat"; then
    echo "Upgrading"
    cp original_dsdt.dat dsdt.dat
else
    echo "Please keep this file 'original_dsdt.dat' for upgrading"
    cp /sys/firmware/acpi/tables/DSDT dsdt.dat
    cp dsdt.dat original_dsdt.dat
fi
