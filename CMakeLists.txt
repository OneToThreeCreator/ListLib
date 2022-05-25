cmake_minimum_required(VERSION 3.0)

# Project name
project(ListLib VERSION 1.0 DESCRIPTION "Simple yet powerful library for managing singly-linked and doubly-linked lists." LANGUAGES C)

if (NOT DEFINED ListLib_BUILD_TESTING)
   option(ListLib_BUILD_TESTING "Build ListLib test program" ON)
endif()

if (NOT DEFINED ListLib_LIB_TYPE)
   set(ListLib_LIB_TYPE SHARED CACHE STRING "Type of library (shared or static) to build ListLib as")
   set_property(CACHE ListLib_LIB_TYPE PROPERTY STRINGS SHARED STATIC)
endif()

if (NOT DEFINED ListLib_INSTALL AND NOT (ListLib_LIB_TYPE MATCHES STATIC))
   option(ListLib_INSTALL "Install ListLib" ON)
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

add_library(list ${ListLib_LIB_TYPE}
   src/list.c
   src/list.h
   include/listlib.h
)

set_target_properties(list PROPERTIES 
   VERSION ${ListLib_VERSION}
   SOVERSION 1
)

if(ListLib_BUILD_TESTING)
   include(CTest)
   enable_testing()
   add_executable(listlib-test src/test.c)
   target_link_libraries(listlib-test list)
   add_test(NAME listlib-test COMMAND listlib-test)
endif()

if (NOT (ListLib_LIB_TYPE MATCHES STATIC) AND ListLib_INSTALL)
   configure_file(listlib.pc.in listlib.pc @ONLY)

   install(TARGETS list
      EXPORT  ListLibTargets
      LIBRARY DESTINATION lib
   )
   
   include(CMakePackageConfigHelpers)
   
   configure_package_config_file(${ListLib_SOURCE_DIR}/Config.cmake.in
      "${ListLib_BINARY_DIR}/ListLibConfig.cmake"
      INSTALL_DESTINATION "lib/cmake/ListLib"
      NO_SET_AND_CHECK_MACRO
      NO_CHECK_REQUIRED_COMPONENTS_MACRO
   )
   
   write_basic_package_version_file(
      "${ListLib_BINARY_DIR}/ListLibConfigVersion.cmake"
      VERSION "${ListLib_VERSION_MAJOR}.${ListLib_VERSION_MINOR}"
      COMPATIBILITY AnyNewerVersion
   )
   install(FILES ${ListLib_BINARY_DIR}/listlib.pc DESTINATION lib/pkgconfig)
   install(EXPORT ListLibTargets
      FILE ListLibTargets.cmake
      DESTINATION lib/cmake/ListLib
   )
   
   install(FILES
      ${ListLib_BINARY_DIR}/ListLibConfig.cmake
      ${ListLib_BINARY_DIR}/ListLibConfigVersion.cmake
      DESTINATION lib/cmake/ListLib
   )

   install(FILES
      ${ListLib_SOURCE_DIR}/include/listlib.h
      DESTINATION include
   )
endif()
