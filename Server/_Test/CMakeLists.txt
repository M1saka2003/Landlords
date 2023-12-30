cmake_minimum_required(VERSION 3.22)

project(Test)

set(CMAKE_CXX_STANDARD 20)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../_bin)

include_directories(./proto)
aux_source_directory(./ SRC)
aux_source_directory(./proto PROTO)
add_executable(protobuf_test ${SRC} ${PROTO})

target_link_libraries(protobuf_test protobuf)