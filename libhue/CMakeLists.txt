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
    FIND_LIBRARY(QTCORE QtCore4)
    FIND_LIBRARY(QTNETWORK QtNetwork4)
    FIND_LIBRARY(QTGUI QtGui4)
    #FIND_LIBRARY(QTDECLARATIVE QtDeclarative4)
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
    target_link_libraries(hue qjson ${QTCORE} ${QTNETWORK} ${QTGUI})
elseif(HARMATTAN_BUILD)
    target_link_libraries(hue qjson)
else()
    qt5_use_modules(hue Gui Network)
endif()

