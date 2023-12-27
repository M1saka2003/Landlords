cmake_minimum_required(VERSION 3.22)
project(Http)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(./ SRC)

add_library(Http STATIC ${SRC})