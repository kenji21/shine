#!/bin/sh

set -x
set -e

#export LDFLAGS=-L/usr/local/opt/qt5/lib
#export CPPFLAGS=-I/usr/local/opt/qt5/include

# qjson no more needed with Qt5
#mkdir -p build-desktop-qjson
#mkdir -p qjson-install

#cd build-desktop-qjson

#cmake -DCMAKE_INSTALL_PREFIX=../qjson-install ../qjson
#make
#make install

#cd ..

mkdir -p build-desktop

cd build-desktop

cmake .. -DDESKTOP_BUILD=1 -DCMAKE_PREFIX_PATH=/usr/local//Cellar/qt5/5.5.1_2/lib/cmake -DWITH_QT5=1
make
