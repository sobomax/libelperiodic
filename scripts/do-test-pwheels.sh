#!/bin/sh

set -e
set -x

uname -a
ARCH=`uname -m`
COMPILER=${COMPILER:-gcc}
. $(dirname $0)/build.conf.sub

${CC} --version

_TCMD="/usr/bin/time"

linux_time()
{
  "${_TCMD}" -f "\t%e real\t%U user\t%s sys" "${@}"
}

if "${_TCMD}" -f "" echo 2>/dev/null >/dev/null
then
  TCMD="linux_time"
else
  TCMD="${_TCMD}"
fi

python3 setup.py build
python3 setup.py sdist
python3 setup.py bdist_wheel
sudo python3 setup.py install

${TCMD} -o ElPeriodic.timings python3 python/ElPeriodic.py
