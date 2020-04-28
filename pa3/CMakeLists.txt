cmake_minimum_required(VERSION 3.15)
project(pa3 C)

set(CMAKE_C_STANDARD 99)

add_executable(pa3 main.c common.h ipc.h log.c log.h banking.h bank_robbery.c pa2345.h pa1.h)

find_library(libruntime libruntime.so paths ${PROJECT_SOURCE_DIR})
target_link_libraries(pa3 ${libruntime})