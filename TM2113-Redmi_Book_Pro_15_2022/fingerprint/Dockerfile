FROM ubuntu:22.04
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Stockholm
RUN apt update && apt-get install -y libssl3 \
  libopencv-dev \
  libusb-1.0-0 \
  fprintd \
  libevent-core-2.1-7 \
  libdbus-1-3
ARG UNAME
ARG UID=1000
ARG GID=1000
RUN groupadd -g $GID -o $UNAME
RUN useradd -m -u $UID -g $GID -o -s /bin/bash $UNAME

#maybe change directory
WORKDIR /app
COPY fingerpp /app/fingerpp

ENTRYPOINT ["/app/fingerpp", "--bus=system"]
