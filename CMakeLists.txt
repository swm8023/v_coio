project(conet)

cmake_minimum_required(VERSION 3.0)


# build config
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# options

set(BUILD_SHARED_LIBS "Build shared libs" OFF)


#

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(conet)

add_executable(main main.cc)
target_link_libraries(main conet)


