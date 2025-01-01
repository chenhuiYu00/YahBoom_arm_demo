cmake_minimum_required(VERSION 3.5)

project(YahBoom LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS
        Widgets
        Network
        REQUIRED)

find_package(OpenCV REQUIRED)

if (ANDROID)
    add_library(YahBoom SHARED
            src/main.cpp
            src/base.cpp
            include/base.h
            src/base.ui
            )
else ()
    add_executable(YahBoom
            src/main.cpp
            src/base.cpp
            src/videowidget.cpp
            src/pythonThread.cpp
            include/base.h
            include/videowidget.h
            include/pythonThread.h
            src/base.ui
            src/videowidget.ui
            )
endif ()

add_executable(tcpTest
        src/utils/tcptestnode.cpp
        )

target_link_libraries(YahBoom PRIVATE
        Qt5::Widgets
        Qt5::Network
        ${OpenCV_LIBS}
        )

target_link_libraries(tcpTest PRIVATE
        Qt5::Widgets
        Qt5::Network
        ${OpenCV_LIBS}
        )
