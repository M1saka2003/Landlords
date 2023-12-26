cmake_minimum_required(VERSION 3.5)

project(Player)

aux_source_directory(./ SRC)

add_library(Player STATIC ${SRC})

target_link_libraries(Player PRIVATE Qt${QT_VERSION_MAJOR}::Core)
