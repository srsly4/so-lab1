cmake_minimum_required (VERSION 2.6)
project (ContactBook)
SET(CMAKE_BUILD_TYPE Debug)

add_library(ContactLib contactlib.c)
add_executable(ContactBook main.c)
target_link_libraries(ContactBook ContactLib)