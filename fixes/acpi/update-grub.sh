 #!/bin/sh

if which update-bootloader >/dev/null 2>&1; then
    update-bootloader
elif which grub2-mkconfig >/dev/null 2>&1; then
    grub2-mkconfig -o /boot/grub2/grub.cfg
elif which grub-mkconfig >/dev/null 2>&1; then
    grub-mkconfig -o /boot/grub/grub.cfg
elif which update-grub >/dev/null 2>&1; then
    update-grub
fi
