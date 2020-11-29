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

${PYTHON_CMD} setup.py build
${PYTHON_CMD} setup.py sdist
if [ "`basename ${PYTHON_CMD}`" != "python2.7" ]
then
  ${PYTHON_CMD} setup.py bdist_wheel
fi
sudo ${PYTHON_CMD} setup.py install
sudo find /usr -name _elp\*.so

${TCMD} -o ElPeriodic.timings ${PYTHON_CMD} tests/t_ElPeriodic.py
