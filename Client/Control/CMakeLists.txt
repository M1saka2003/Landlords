cmake_minimum_required(VERSION 3.5)

project(Control)

aux_source_directory(./ SRC)

add_library(Control STATIC ${SRC})

target_link_libraries(Control PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Multimedia
)

