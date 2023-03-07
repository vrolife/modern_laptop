# Build

```
Build image

sudo podman build -t fingerprint -f Dockerfile --build-arg UNAME=$USER

Get device path

DEV_PATH=$(lsusb -d 10a5:9201 | awk '{print $2"/"$4}' | tr -d :)

sudo mkdir -p /opt/fingerprint-ocv

sudo cp fingerprint.service.template /opt/fingerprint-ocv/fingerprint.service
sudo cp stop_fprint.sh /opt/fingerprint-ocv/stop_fprint.sh

sudo sed -i "s|<device path>|/dev/bus/usb/$DEV_PATH|g" /opt/fingerprint-ocv/fingerprint.service

sudo systemctl enable /opt/fingerprint-ocv/fingerprint.service

```

