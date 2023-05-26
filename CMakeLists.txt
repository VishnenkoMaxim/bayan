cmake_minimum_required(VERSION 3.18)
include(CMakePrintHelpers)
set(CMAKE_CXX_STANDARD 17)
set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 1.0.${PATCH_VERSION})
project(bayan VERSION ${PROJECT_VERSION})

configure_file(version.h.in version.h)

find_package(Boost COMPONENTS program_options system filesystem regex REQUIRED)
cmake_print_variables(Boost_INCLUDE_DIR)
cmake_print_variables(Boost_LIBRARIES)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${Boost_INCLUDE_DIR})
include_directories(headers)

link_directories(${CMAKE_BINARY_DIR})
link_directories(lib)

add_library(functions src/bayan.cpp)
add_executable(bayan main.cpp)

target_link_libraries(bayan PRIVATE stdc++ m functions ${Boost_LIBRARIES})

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