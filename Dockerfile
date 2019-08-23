FROM ubuntu:18.04

# Install build dependencies
RUN apt-get update && apt-get -y install sudo build-essential python python3 \
	uuid-dev nasm openssl gcc-multilib git m4 bison flex

# Install ACPICA Utilities
ADD http://ftp.br.debian.org/debian/pool/main/a/acpica-unix/acpica-tools_20160831-1+b1_amd64.deb /tmp
RUN cd /tmp && \
    dpkg -i acpica-tools_20160831-1+b1_amd64.deb && \
    apt-get install -f && \
    apt-get -y install python3-distutils

# Add a user account, give sudo access
RUN useradd -m docker && echo "docker:docker" | chpasswd && adduser docker sudo

USER docker
RUN git config --global user.name "User" && git config --global user.email "user@example.com"
CMD /bin/bash
