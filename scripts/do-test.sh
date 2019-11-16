#!/bin/sh

set -e
set -x

CC=${CC:-cc}

uname -a
${CC} --version

./configure
make all
./configure --enable-coverage
make clean all
cd src
for arg in '' '-w' '-p' '-p -w'
do
  ./testskew ${arg} -L 200 5
  ./testskew ${arg} 10 5.0 20 2.5 40 2.5 80 2.5 160 2.5 320 2.5
  if [ "`uname -m`" != "arm64" ]
  then
    ./testskew ${arg} -f 0.75 -v 0.1 100
  fi
  sh ./findskew.sh ${arg}
done
