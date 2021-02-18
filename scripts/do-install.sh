#!/bin/sh

set -e

PKGS="python3-pip"

. $(dirname $0)/build.conf.sub

if [ ! -z "${PRE_INSTALL_CMD}" ]
then
        ${PRE_INSTALL_CMD}
fi

PIP_CMD="${PYTHON_CMD} -m pip"

sudo -H DEBIAN_FRONTEND=noninteractive apt-get update -y
sudo -H DEBIAN_FRONTEND=noninteractive apt-get -y install ${PKGS}
#sudo ${PIP_CMD} install --upgrade pip
${PIP_CMD} install --user setuptools
# Does not work at the moment:
#  https://github.com/sobomax/libelperiodic/runs/1930614303
#${PIP_CMD} install --user cpp-coveralls

if [ ! -z "${POST_INSTALL_CMD}" ]
then
        ${POST_INSTALL_CMD}
fi
