FROM ubuntu:18.04
RUN apt-get update && apt-get -y install sudo build-essential python \
	uuid-dev nasm openssl gcc-multilib git m4 bison flex

# Install ACPICA Utilities
ADD https://acpica.org/sites/acpica/files/acpica-unix-20160422.tar.gz /tmp
RUN cd /tmp && \
    tar -xvf acpica-unix-20160422.tar.gz && \
    cd acpica-unix-20160422 && \
    make && \
    make install && \
    rm -fr * 

RUN useradd -m docker && echo "docker:docker" | chpasswd && adduser docker sudo

USER docker
CMD /bin/bash
