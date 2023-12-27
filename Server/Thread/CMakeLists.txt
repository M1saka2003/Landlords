cmake_minimum_required(VERSION 3.22)
project(Thread)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(./ SRC)

add_library(Thread STATIC ${SRC})