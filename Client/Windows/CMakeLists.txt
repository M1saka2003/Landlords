cmake_minimum_required(VERSION 3.5)

project(Windows)

aux_source_directory(./ SRC)

file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/*.h)

file(GLOB UIS ${PROJECT_SOURCE_DIR}/*.ui)

add_library(Windows STATIC ${SRC} ${HEADERS} ${UIS})

target_link_libraries(Windows PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
)
