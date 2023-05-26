cmake_minimum_required(VERSION 3.18)
include(CMakePrintHelpers)
set(CMAKE_CXX_STANDARD 17)
set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 1.0.${PATCH_VERSION})
project(bayan VERSION ${PROJECT_VERSION})

configure_file(version.h.in version.h)

#find_package(Boost COMPONENTS program_options system filesystem regex REQUIRED)
cmake_print_variables(Boost_INCLUDE_DIR)
cmake_print_variables(Boost_LIBRARIES)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
#include_directories(${Boost_INCLUDE_DIR})
include_directories(headers)
include_directories(/usr/local/boost_1_82_0)

link_directories(${CMAKE_BINARY_DIR})
link_directories(lib)

add_library(program_options STATIC IMPORTED)
add_library(system STATIC IMPORTED)
add_library(filesystem STATIC IMPORTED)
add_library(regex STATIC IMPORTED)

set_property(TARGET program_options PROPERTY IMPORTED_LOCATION  /usr/local/boost_1_82_0/lib/libboost_program_options.a)
set_property(TARGET system PROPERTY IMPORTED_LOCATION           /usr/local/boost_1_82_0/lib/libboost_system.a)
set_property(TARGET filesystem PROPERTY IMPORTED_LOCATION       /usr/local/boost_1_82_0/lib/libboost_filesystem.a)
set_property(TARGET regex PROPERTY IMPORTED_LOCATION            /usr/local/boost_1_82_0/lib/libboost_regex.a)

add_library(functions src/bayan.cpp)
add_executable(bayan main.cpp)

#target_link_libraries(bayan PRIVATE stdc++ m functions ${Boost_LIBRARIES})
target_link_libraries(bayan PRIVATE stdc++ m functions program_options system filesystem regex)

set_target_properties(bayan functions PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_compile_options(bayan PRIVATE -Wall -Wextra -pedantic)
target_compile_options(functions PRIVATE -Wall -Wextra -pedantic)

install(TARGETS bayan RUNTIME DESTINATION bin)
set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Vishnenko Maxim")

include(CPack)