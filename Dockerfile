FROM ubuntu:18.04

# Install build dependencies
RUN apt-get update && apt-get -y install sudo build-essential python \
	uuid-dev nasm openssl gcc-multilib git m4 bison flex

# Install ACPICA Utilities
ADD https://acpica.org/sites/acpica/files/acpica-unix-20160422.tar.gz /tmp
RUN cd /tmp && \
    tar -xvf acpica-unix-20160422.tar.gz && \
    cd acpica-unix-20160422 && \
    make && make install && \
    cd /tmp && rm -rf acpica-unix-20160422 acpica-unix-20160422.tar.gz

# Add a user account, give sudo access
RUN useradd -m docker && echo "docker:docker" | chpasswd && adduser docker sudo

USER docker
RUN git config --global user.name "User" && git config --global user.email "user@example.com"
CMD /bin/bash
