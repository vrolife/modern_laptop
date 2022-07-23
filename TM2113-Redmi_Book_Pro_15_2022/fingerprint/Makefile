build:
mkdir -p ~/.podman_apt_cache
podman build -t fingerprint -v ~/.podman_apt_cache:/var/cache/apt .
clean:
rm -f ~/.podman_apt_cache/pkgcache.bin
rm -f ~/.podman_apt_cache/srcpkgcache.bin
rm -f ~/.podman_apt_cache/archives/lock
install:
cp ./stop_fprint.sh /usr/bin/stop_fprint
cp ./fingerprint.service /etc/systemd/system
systemctl enable --now fingerprint
