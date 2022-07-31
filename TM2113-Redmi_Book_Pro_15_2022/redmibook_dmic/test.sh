modprobe -r snd-soc-acp6x-mach

dkms remove soundcard/1.0.0
dkms install soundcard/1.0.0

modprobe snd-soc-acp6x-mach
dmesg | tail
