#!/bin/sh

SCRIPT_DIR="$(dirname $(realpath $0))"

FILE="$1"
TEMP="$(mktemp)"

#head -n 3 "$FILE" > "$TEMP"

cat "$SCRIPT_DIR/fprintd-override.service" >> "$TEMP"
cp "$TEMP" "$FILE"

rm -f "$TEMP"
