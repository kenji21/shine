cmake_minimum_required(VERSION 2.8.9)

project(shine C CXX)

set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-permissive -pedantic -Wall -Wextra -fPIC")

if(UNIX)
  include(FindPkgConfig)
else() # win32
endif()

if(DESKTOP_BUILD)
    set(QT4_BUILD 1)
    if(UNIX)
      include_directories(qjson-install/include)
      link_directories(qjson-install/lib)
    else()
      include_directories(qjson-install-win/include)
      link_directories(qjson-install-win/lib)
    endif()
elseif(HARMATTAN_BUILD)
    set(QT4_BUILD 1)
endif()

if(NOT QT4_BUILD)
    find_package(Qt5Core)
    find_package(Qt5Qml)
    find_package(Qt5Quick)
else()
    find_package(Qt4 REQUIRED)

    set(QT_USE_QTDECLARATIVE ON)
    set(QT_USE_QTNETWORK ON)
    set(QT_USE_QTCORE ON)

    include( ${QT_USE_FILE} )
endif()

add_subdirectory(libhue)
add_subdirectory(plugin)
add_subdirectory(apps)


