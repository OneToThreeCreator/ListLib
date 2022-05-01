cmake_minimum_required(VERSION 3.0)

# Project name
project(ListLib VERSION 1.0 DESCRIPTION "Simple yet powerful library for managing singly-linked and doubly-linked lists." LANGUAGES C)

option(ListLib_BUILD_TESTING         "Build ListLib test program" ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

add_library(listlib STATIC
   src/list.c
   src/list.h
)

set_target_properties(listlib PROPERTIES 
	VERSION ${ListLib_VERSION}
   SOVERSION 1
)

if(ListLib_BUILD_TESTING)
   include(CTest)
   enable_testing()
   add_executable(listlib-test src/test.c)
   target_link_libraries(listlib-test listlib)
   add_test(NAME listlib-test COMMAND listlib-test)
endif()


