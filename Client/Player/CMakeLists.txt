cmake_minimum_required(VERSION 3.5)

project(Player)

aux_source_directory(./ SRC)

file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/*.h)

add_library(${PROJECT_NAME} STATIC ${SRC} ${HEADERS})

target_link_libraries(Player PRIVATE Qt${QT_VERSION_MAJOR}::Core)
