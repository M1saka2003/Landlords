cmake_minimum_required(VERSION 3.5)

project(Strategy)

aux_source_directory(./ SRC)

add_library(Strategy STATIC ${SRC})

target_link_libraries(Strategy PRIVATE Qt${QT_VERSION_MAJOR}::Core)
