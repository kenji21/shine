mkdir build-desktop-qjson-win-debug
mkdir qjson-install-win-debug

cd build-desktop-qjson-win-debug

cmake.exe -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../qjson-install-win-debug ../qjson

mingw32-make.exe
mingw32-make.exe install

cd ..

mkdir build-desktop-win-debug
cd build-desktop-win-debug

cmake.exe -G "MinGW Makefiles" .. -DDESKTOP_BUILD=1 -DCMAKE_BUILD_TYPE=Debug

mingw32-make.exe

cd ..
