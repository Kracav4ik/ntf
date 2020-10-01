cmake_minimum_required(VERSION 3.16)
project(ntf)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(SOURCES
        colors.h
        DiskPopup.cpp
        DiskPopup.h
        FilePanel.cpp
        FilePanel.h
        Lines.cpp
        Lines.h
        main.cpp
        MessagePopup.cpp
        MessagePopup.h
        Screen.cpp
        Screen.h
)

add_executable(ntf ${SOURCES})
