cmake_minimum_required (VERSION 2.6)
project (ContactBook)

add_library(ContactLib contactlib.c)
add_executable(ContactBook main.c)
target_link_libraries(ContactBook ContactLib)