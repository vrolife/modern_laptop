#/bin/sh

if test -z "$TOP_DIR"; then
    export TOP_DIR="$(realpath "$(dirname "$(realpath $0)")/../..")"
fi

BACKUP_DIR="$1"

if test -e "$TOP_DIR/original_dsdt.dat"; then
    mkdir -p "$BACKUP_DIR"
    (cd "$BACKUP_DIR"; acpidump -b; rm -f dsdt.dat)
    cp "$TOP_DIR/original_dsdt.dat" "$BACKUP_DIR/dsdt.dat"
    rm "$TOP_DIR/original_dsdt.dat"
else
    if test ! -e "$BACKUP_DIR"; then
        mkdir -p "$BACKUP_DIR"
        (cd "$BACKUP_DIR"; acpidump -b)
    fi
fi
