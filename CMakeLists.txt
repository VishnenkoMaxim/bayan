cmake_minimum_required(VERSION 3.18)
include(CMakePrintHelpers)
set(CMAKE_CXX_STANDARD 17)
set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 1.0.${PATCH_VERSION})
set(PROJECT_NAME bayan)

project(${PROJECT_NAME} VERSION ${PROJECT_VERSION})

configure_file(version.h.in version.h)

find_package(Boost REQUIRED COMPONENTS filesystem system program_options regex json)

set(EXT_LIB_LIST
        Boost::filesystem
        Boost::system
        Boost::program_options
        Boost::regex
        Boost::json
)

cmake_print_variables(Boost_INCLUDE_DIR)
cmake_print_variables(Boost_LIBRARIES)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(headers)

link_directories(${CMAKE_BINARY_DIR})

add_library(search_engine src/CHashReader.cpp src/CDuplicatedFile.cpp src/CSearcher.cpp)
add_library(gui_interface src/gui_interface/CBoostSocketGUI.cpp)

add_executable(${PROJECT_NAME} main.cpp src/data_transformer/CJSONDataTransformer.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE search_engine gui_interface ${EXT_LIB_LIST})

set_target_properties(${PROJECT_NAME} search_engine gui_interface PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
target_compile_options(search_engine PRIVATE -Wall -Wextra -pedantic)
target_compile_options(gui_interface PRIVATE -Wall -Wextra -pedantic)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Vishnenko Maxim")

include(CPack)