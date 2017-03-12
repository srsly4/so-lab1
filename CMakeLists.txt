cmake_minimum_required (VERSION 2.6)
project (contactbook)
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_C_STANDARD 99)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(contactbook include)

#libraries
add_library(contact-static STATIC EXCLUDE_FROM_ALL lib/contact.c)
add_library(contact-shared SHARED EXCLUDE_FROM_ALL lib/contact.c)

add_executable(contactbook src/main.c)
target_link_libraries(contactbook contact-static)

add_executable(contactbook-static EXCLUDE_FROM_ALL src/main.c)
target_link_libraries(contactbook-static contact-static)
add_dependencies(contactbook-static contact-static)

add_executable(contactbook-shared EXCLUDE_FROM_ALL src/main.c)
add_dependencies(contactbook-shared contact-shared)
target_link_libraries(contactbook-shared contact-shared)

add_executable(contactbook-dynamic EXCLUDE_FROM_ALL src/main.c)
target_link_libraries(contactbook-dynamic contact-shared)
target_compile_definitions(contactbook-dynamic PRIVATE DYNLIB=true)
target_compile_options(contactbook-dynamic PRIVATE -rdynamic)
target_link_libraries(contactbook-dynamic dl)

add_definitions("-Wall")
install(TARGETS ${PROJECT_NAME} DESTINATION lib/${PROJECT_NAME})

#test
add_custom_target(
        tests
        COMMAND sh ./test-runner.sh
        DEPENDS contactbook-static contactbook-shared contactbook-dynamic
)

#optimization levels
add_library(contact-O0 STATIC EXCLUDE_FROM_ALL lib/contact.c)
add_library(contact-O1 STATIC EXCLUDE_FROM_ALL lib/contact.c)
add_library(contact-O2 STATIC EXCLUDE_FROM_ALL lib/contact.c)
add_library(contact-O3 STATIC EXCLUDE_FROM_ALL lib/contact.c)
add_library(contact-Os STATIC EXCLUDE_FROM_ALL lib/contact.c)
target_compile_options(contact-O0 PRIVATE "-O0")
target_compile_options(contact-O1 PRIVATE "-O1")
target_compile_options(contact-O2 PRIVATE "-O2")
target_compile_options(contact-O3 PRIVATE "-O3")
target_compile_options(contact-Os PRIVATE "-Os")
add_executable(contactbook-O0 EXCLUDE_FROM_ALL src/main.c)
add_executable(contactbook-O1 EXCLUDE_FROM_ALL src/main.c)
add_executable(contactbook-O2 EXCLUDE_FROM_ALL src/main.c)
add_executable(contactbook-O3 EXCLUDE_FROM_ALL src/main.c)
add_executable(contactbook-Os EXCLUDE_FROM_ALL src/main.c)
target_link_libraries(contactbook-O0 contact-O0)
target_link_libraries(contactbook-O1 contact-O1)
target_link_libraries(contactbook-O2 contact-O2)
target_link_libraries(contactbook-O3 contact-O3)
target_link_libraries(contactbook-Os contact-Os)
target_compile_options(contactbook-O0 PRIVATE "-O0")
target_compile_options(contactbook-O1 PRIVATE "-O1")
target_compile_options(contactbook-O2 PRIVATE "-O2")
target_compile_options(contactbook-O3 PRIVATE "-O3")
target_compile_options(contactbook-Os PRIVATE "-Os")

add_custom_target(
        optimizations
        DEPENDS contactbook-O0 contactbook-O1 contactbook-O2 contactbook-O3 contactbook-Os
)
add_custom_target(
        optimization-tests
        COMMAND sh ./optimization-test-runner.sh
        DEPENDS optimizations
)
