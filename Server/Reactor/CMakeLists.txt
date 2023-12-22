cmake_minimum_required(VERSION 3.22)
project(Reactor)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(./ SRC)
# Éú³É¾²Ì¬¿â
add_library(Reactor STATIC ${SRC})