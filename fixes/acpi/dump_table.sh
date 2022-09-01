#/bin/sh

TABLE="$1"
TABLE_UPPER="$(echo $TABLE|tr [:lower:] [:upper:])"

if test -z "$TOP_DIR"; then
    export TOP_DIR="$(realpath "$(dirname "$(realpath $0)")/../..")"
fi

if test -e "$TOP_DIR/original_$TABLE.dat"; then
    echo "Upgrading"
    cp "$TOP_DIR/original_$TABLE.dat" $TABLE.dat
else
    printf "\e[1;33mPlease keep this file '$TOP_DIR/original_$TABLE.dat' for future upgrade\e[0m\n"
    cp /sys/firmware/acpi/tables/$TABLE_UPPER $TABLE.dat
    cp $TABLE.dat "$TOP_DIR/original_$TABLE.dat"
fi
