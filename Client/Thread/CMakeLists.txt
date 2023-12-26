cmake_minimum_required(VERSION 3.5)

project(Thread)

aux_source_directory(./ SRC)

add_library(Thread STATIC ${SRC})

target_link_libraries(Thread PRIVATE Qt${QT_VERSION_MAJOR}::Core)
