#!/bin/sh

set -e
set -x

CC=${CC:-cc}

uname -a
ARCH=`uname -m`
${CC} --version

./configure
make all
./configure --enable-coverage
make clean all

if [ "${ARCH}" = "arm64" ]
then
  exit 0
fi

cd src
for arg in '' '-w' '-p' '-p -w'
do
  ./testskew ${arg} -L 200 5
  ./testskew ${arg} 10 5.0 20 2.5 40 2.5 80 2.5 160 2.5 320 2.5
  ./testskew ${arg} -f 0.75 -v 0.1 100
  sh ./findskew.sh ${arg}
done
