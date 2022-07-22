FROM ubuntu:22.04

# Install build dependencies
RUN apt-get update && apt-get -y install sudo build-essential python3 python-is-python3 \
    uuid-dev nasm openssl gcc-multilib git m4 bison flex qemu-system-x86

# Install ACPICA Utilities
RUN apt-get -y install acpica-tools
RUN apt-get -y install python3-distutils

# Install stitching dependencies
RUN apt-get install -y --no-install-recommends libxcb1 \
    libxcb-icccm4 libxcb-image0 libxcb-keysyms1 \
    libxcb-randr0 libxcb-render-util0 libxcb-shape0 \
    libxcb-sync1 libxcb-xfixes0 libxcb-xinerama0 \
    libxkbcommon-x11-0 unzip && \
    apt-get clean

# Install UTF8 as default locale
ENV LANG=en_US.UTF-8
RUN apt-get install --no-install-recommends -y locales && \
    apt-get clean && \
    locale-gen ${LANG} && update-locale LANG=${LANG}

# Add a user account, give sudo access
RUN useradd -m docker && echo "docker:docker" | chpasswd && adduser docker sudo

USER docker
RUN git config --global user.name "User" && git config --global user.email "user@example.com"
CMD /bin/bash
