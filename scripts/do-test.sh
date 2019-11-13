#!/bin/sh

set -e

CC=${CC:-cc}

uname -a
${CC} --version

./configure
make all
./configure --enable-coverage
make clean all
cd src
for arg in '' # '-p'
do
  ./testskew -L 200 5
  sh -x ./findskew.sh ${arg}
done
