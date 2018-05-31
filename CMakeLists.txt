cmake_minimum_required(VERSION 2.8.9)
set(UBUNTU_MANIFEST_PATH "apps/ubuntu/manifest.json" CACHE INTERNAL "Tells QtCreator location and name of the manifest file")

project(shine C CXX)

set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-permissive -pedantic -Wall -Wextra -fPIC")

if(UNIX)
  include(FindPkgConfig)
else() # win32
endif()

if(DESKTOP_BUILD)
    #set(QT4_BUILD 1)
    if(UNIX)
      #include_directories(qjson-install/include)
      #link_directories(qjson-install/lib)
    else()
      #include_directories(qjson-install-win/include)
      if(CMAKE_BUILD_TYPE MATCHES Debug)
      #    link_directories(qjson-install-win-debug/lib)
      else()
      #    link_directories(qjson-install-win/lib)
      endif()
    endif()
elseif(HARMATTAN_BUILD)
    set(QT4_BUILD 1)
endif()

if(NOT QT4_BUILD)
    find_package(Qt5Gui)
    find_package(Qt5Widgets)
    find_package(Qt5Core)
    find_package(Qt5Network)
    #find_package(Qt5Qml)
    #find_package(Qt5Quick)
    #find_package(Qt5Declarative)

    include_directories(${Qt5Widgets_INCLUDE_DIRS})
    include_directories(${Qt5Network_INCLUDE_DIRS})
    #include_directories(${Qt5Quick_INCLUDE_DIRS})
    #include_directories(${Qt5Qml_INCLUDE_DIRS})
    include_directories(${Qt5Core_INCLUDE_DIRS})
    #include_directories(${Qt5Declarative_INCLUDE_DIRS})

    #include( ${QT_USE_FILE} )
else()
    set(QT_ROOT_DIR "C:/Qt/4.8.6")
    set(QT_QMAKE_EXECUTABLE ${QT_ROOT_DIR}/bin/qmake.exe)
    set( ENV(CMAKE_PREFIX_PATH) ${QT_ROOT} )
    find_package(Qt4 REQUIRED)

    set(QT_USE_QTDECLARATIVE ON)
    set(QT_USE_QTNETWORK ON)
    set(QT_USE_QTCORE ON)

    include( ${QT_USE_FILE} )
endif()

add_subdirectory(libhue)
#add_subdirectory(plugin)
#add_subdirectory(apps)
