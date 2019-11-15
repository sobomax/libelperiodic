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
for arg in '' '-w' '-p' '-p -w'
do
  ./testskew ${arg} -L 200 5
  sh ./findskew.sh ${arg}
done
