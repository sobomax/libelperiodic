#!/bin/sh

set -e

. $(dirname $0)/build.conf.sub

PKGS="python3-pip"

if [ ! -z "${PRE_INSTALL_CMD}" ]
then
        ${PRE_INSTALL_CMD}
fi

sudo -H DEBIAN_FRONTEND=noninteractive apt-get update -y
sudo -H DEBIAN_FRONTEND=noninteractive apt-get -y install ${PKGS}
sudo pip3 install --upgrade pip
pip3 install --user setuptools
pip3 install --user cpp-coveralls

if [ ! -z "${PRE_INSTALL_CMD}" ]
then
        ${POST_INSTALL_CMD}
fi
