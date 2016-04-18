mkdir build-desktop-win
cd build-desktop-win

set PATH=%PATH%;C:\Program Files\CMake\bin;C:\Qt\Qt5.5.1\Tools\mingw492_32\bin;C:\Qt\Qt5.5.1\bin;

cmake.exe -G "MinGW Makefiles" .. -DDESKTOP_BUILD=1 -DCMAKE_PREFIX_PATH=C:\Qt\Qt5.5.1\5.5\mingw492_32\lib\cmake -DWITH_QT5=1

mingw32-make.exe

cd ..
