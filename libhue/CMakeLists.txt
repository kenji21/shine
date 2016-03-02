include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(libhue_SRCS
    huebridgeconnection.cpp
    discovery.cpp
    configuration.cpp
    groups.cpp
    group.cpp
    lights.cpp
    lightsfiltermodel.cpp
    light.cpp
    lightinterface.h
)

add_library(hue SHARED ${libhue_SRCS})

if(DESKTOP_BUILD)
  if(WIN32)
    find_package(Qt5Core)
    find_package(Qt5Network)
    find_package(Qt5Gui)
    #find_package(Qt5Declarative)
  else()
    FIND_LIBRARY(QTCORE QtCore)
    FIND_LIBRARY(QTNETWORK QtNetwork)
    FIND_LIBRARY(QTGUI QtGui)
  endif()
  set(libhue_SRCS
      huebridgeconnection.cpp
      discovery.cpp
      configuration.cpp
      groups.cpp
      group.cpp
      lights.cpp
      lightsfiltermodel.cpp
      light.cpp
      lightinterface.h
  )
    target_link_libraries(hue qjson Qt5::Widgets Qt5::Network Qt5::Gui)
elseif(HARMATTAN_BUILD)
    target_link_libraries(hue qjson)
else()
    qt5_use_modules(hue Gui Network)
endif()

