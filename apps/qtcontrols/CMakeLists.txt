include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/libhue
)

file(GLOB_RECURSE QML_SRCS *.qml *.js)

set(shine_SRCS
    QmlApplicationViewer.cpp
    main.cpp
    keystore.cpp

    ${QML_SRCS}
)

set(shine_SRCS ${shine_SRCS})
add_executable(shine ${shine_SRCS})

if(QT4_BUILD)
  FIND_LIBRARY(QTCORE QtCore)
  FIND_LIBRARY(QTNETWORK QtNetwork)
  FIND_LIBRARY(QTGUI QtGui)
 FIND_LIBRARY(QTDECLARATIVE QtDeclarative)
    target_link_libraries(shine hue ${QTCORE} ${QTNETWORK} ${QTGUI} ${QTDECLARATIVE})
else()
    qt5_use_modules(shine Gui Qml Quick Widgets)
    target_link_libraries(shine hue)
endif()

add_custom_target(shine-qmlfiles ALL
    COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/qml ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${QMLFILES}
)
add_custom_target(shine-icon ALL
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/shine.svg ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${QMLFILES}
)
