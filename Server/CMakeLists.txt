cmake_minimum_required(VERSION 3.22)
project(Server)

set(CMAKE_CXX_STANDARD 20)

include_directories(
    ${PROJECT_SOURCE_DIR}/Currency
    ${PROJECT_SOURCE_DIR}/Http
    ${PROJECT_SOURCE_DIR}/Reactor
    ${PROJECT_SOURCE_DIR}/Tcp
    ${PROJECT_SOURCE_DIR}/Thread

    ${PROJECT_SOURCE_DIR}/_Test
)

add_subdirectory(Currency)
add_subdirectory(Http)
add_subdirectory(Reactor)
add_subdirectory(Tcp)
add_subdirectory(Thread)

add_subdirectory(_Test)

link_libraries(Tcp Http Reactor Thread Currency)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/_bin)

add_executable(Server main.cpp)