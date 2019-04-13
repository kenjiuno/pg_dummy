FROM postgres:10

RUN apt-get update
RUN apt-get -y upgrade
RUN apt-get dist-upgrade

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get -y install g++ postgresql-server-dev-10 cmake
RUN apt-get -y install reprepro
RUN apt-get -y install dpkg-sig
