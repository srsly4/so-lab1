cmake_minimum_required (VERSION 2.6)
project (contactbook)
set(CMAKE_BUILD_TYPE Debug)
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

install(TARGETS ${PROJECT_NAME} DESTINATION lib/${PROJECT_NAME})

add_custom_target(
        tests
        COMMAND sh ./test-runner.sh
        DEPENDS contactbook-static contactbook-shared contactbook-dynamic
)