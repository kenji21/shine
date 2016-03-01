#!/bin/sh

set -x
set -e

mkdir -p build-desktop-qjson
mkdir -p qjson-install

cd build-desktop-qjson

cmake -DCMAKE_INSTALL_PREFIX=../qjson-install ../qjson
make
make install

cd ..

mkdir -p build-desktop

cd build-desktop

cmake .. -DDESKTOP_BUILD=1
make

