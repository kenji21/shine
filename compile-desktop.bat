mkdir build-desktop-qjson-win
mkdir qjson-install-win

cd build-desktop-win

"c:\Program Files\CMake 2.8\bin\cmake.exe" -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=../qjson-install-win ../qjson

make
make install

cd ..

mkdir build-desktop-win

cd build-desktop-win

"c:\Program Files\CMake 2.8\bin\cmake.exe" -G "MinGW Makefiles" .. -DDESKTOP_BUILD=1

make