include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(libhue_SRCS
    huebridgeconnection.cpp
    hueobject.cpp
    huemodel.cpp
    discovery.cpp
    configuration.cpp
    groups.cpp
    group.cpp
    lights.cpp
    lightsfiltermodel.cpp
    light.cpp
    lightinterface.h
    scenes.cpp
    scene.cpp
    schedules.cpp
    schedule.cpp
    scenesfiltermodel.cpp
    schedulesfiltermodel.cpp
    sensor.cpp
    sensors.cpp
    sensorsfiltermodel.cpp
    rule.cpp
    rules.cpp
    rulesfiltermodel.cpp
    action.cpp
    condition.cpp
)

add_library(hue SHARED ${libhue_SRCS})

if(DESKTOP_BUILD)
  #set(QT4_BUILD 1)
  if(WIN32)
    if(NOT QT4_BUILD)
      find_package(Qt5Core)
      find_package(Qt5Network)
      find_package(Qt5Gui)
      find_package(Qt5Widgets)
      #find_package(Qt5Declarative)
    else()
	set(QT_ROOT_DIR "C:/Qt/4.8.6")
	set(QT_QMAKE_EXECUTABLE ${QT_ROOT_DIR}/bin/qmake.exe)
	set( ENV(CMAKE_PREFIX_PATH) ${QT_ROOT} )
    	find_package(Qt4 REQUIRED)

	#FIND_LIBRARY(QTCORE QtCore4)
   	#FIND_LIBRARY(QTNETWORK QtNetwork4)
   	#FIND_LIBRARY(QTGUI QtGui4)
    endif()
  else()
    FIND_LIBRARY(QTCORE QtCore4)
    FIND_LIBRARY(QTNETWORK QtNetwork4)
    FIND_LIBRARY(QTGUI QtGui4)
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

      if(CMAKE_BUILD_TYPE MATCHES Debug)
        if(NOT QT4_BUILD)
          target_link_libraries(hue Qt5::Widgets Qt5::Network Qt5::Gui)
        else()
          target_link_libraries(hue qjsond ${QTCORE} ${QTNETWORK} ${QTGUI})
	  #target_link_libraries(hue qjsond Qt4::Network Qt4::Gui Qt4::Core)
        endif()
      else()
        if(NOT QT4_BUILD)
           target_link_libraries(hue Qt5::Widgets Qt5::Network Qt5::Gui)
        else()
           target_link_libraries(hue qjson Qt4::QtNetwork Qt4::QtGui Qt4::QtCore)
	   #target_link_libraries(hue qjson ${QTCORE} ${QTNETWORK} ${QTGUI})
        endif()
      endif()

elseif(HARMATTAN_BUILD)
    target_link_libraries(hue qjson)
else()
    qt5_use_modules(hue Gui Network)
endif()
