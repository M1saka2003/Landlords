cmake_minimum_required(VERSION 3.5)

project(Client VERSION 0.1 LANGUAGES CXX)
# 自动处理.ui文件,最后生成一个ui_xxx.h文件
set(CMAKE_AUTOUIC ON)
# 自动处理元对线相关的宏 Q_OBJECT
set(CMAKE_AUTOMOC ON)
# 自动处理 qrc 文件
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)



set(PROJECT_SOURCES
        main.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/Card
    ${PROJECT_SOURCE_DIR}/Windows
    ${PROJECT_SOURCE_DIR}/Control
    ${PROJECT_SOURCE_DIR}/Player
    ${PROJECT_SOURCE_DIR}/Strategy
    ${PROJECT_SOURCE_DIR}/Thread
)

add_subdirectory(Card)
add_subdirectory(Windows)
add_subdirectory(Control)
add_subdirectory(Player)
add_subdirectory(Strategy)
add_subdirectory(Thread)

# 加载资源文件
qt5_add_resources(RES res.qrc)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Client
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Client APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Client SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()

        add_executable(Client
            ${PROJECT_SOURCES}
            ${RES}
        )
    endif()
endif()

target_link_libraries(Client PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    Card Windows Control Player Strategy Thread
)

