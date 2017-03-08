cmake_minimum_required (VERSION 2.6)
project (ContactBook)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_STANDARD 99)

add_library(ContactLib contactlib.c)
add_executable(ContactBook main.c)
target_link_libraries(ContactBook ContactLib)