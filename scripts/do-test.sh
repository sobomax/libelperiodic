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
  sh -x ./findskew.sh ${arg}
done
