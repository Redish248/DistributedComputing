cmake_minimum_required(VERSION 3.13)
project(pa1 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_BUILD_TYPE Debug)


set(MAIN main.c log.c)

add_executable(pa1 ${MAIN})
