cmake_minimum_required(VERSION 3.15)
project(pa4 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_BUILD_TYPE Debug)

add_executable(pa4 main.c common.h ipc.h log.c log.h banking.h pa2345.h pa1.h priorityQueue.c priorityQueue.h)

find_library(libruntime libruntime.so paths ${PROJECT_SOURCE_DIR})
target_link_libraries(pa4 ${libruntime})