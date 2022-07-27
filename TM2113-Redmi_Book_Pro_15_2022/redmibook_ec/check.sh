#!/bin/sh

for FN in /sys/kernel/redmibook_ec/*; do
    echo $FN $(cat $FN)
done
