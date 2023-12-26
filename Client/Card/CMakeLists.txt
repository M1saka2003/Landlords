cmake_minimum_required(VERSION 3.5)

project(Card)

aux_source_directory(./ SRC)

add_library(Card STATIC ${SRC})

target_link_libraries(Card PRIVATE Qt${QT_VERSION_MAJOR}::Core)
