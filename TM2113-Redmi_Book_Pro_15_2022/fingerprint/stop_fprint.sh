ID=$(podman ps -q  --filter ancestor=localhost/fingerprint:latest)
podman kill $ID
podman container rm $ID

