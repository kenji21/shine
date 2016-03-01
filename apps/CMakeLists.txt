if(HARMATTAN_BUILD)
    add_subdirectory(harmattan)
elseif(DESKTOP_BUILD)
    add_subdirectory(qtcontrols)
else()
    add_subdirectory(ubuntu)
    add_subdirectory(qtcontrols)
endif()

