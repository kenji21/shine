mkdir build-desktop-qjson-win
mkdir qjson-install-win

cd build-desktop-qjson-win

cmake.exe -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=../qjson-install-win ../qjson

mingw32-make.exe
mingw32-make.exe install

cd ..

mkdir build-desktop-win
cd build-desktop-win

cmake.exe -G "MinGW Makefiles" .. -DDESKTOP_BUILD=1
:: -DCMAKE_PREFIX_PATH=C:\Qt\Qt5.5.1\5.5\mingw492_32\bin -DWITH_QT5=1

mingw32-make.exe

cd ..

pause