project(coio)

cmake_minimum_required(VERSION 3.0)


# build config
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# options
set(BUILD_SHARED_LIBS "Build shared libs" OFF)
#


include_directories(3rdparty/install/include)
include_directories(${PROJECT_SOURCE_DIR})

link_directories(3rdparty/install/lib)
link_directories(${PROJECT_BINARY_DIR}/lib)

add_subdirectory(3rdparty)
add_subdirectory(coio)

add_executable(main main.cc)
target_link_libraries(main vcoio vco ev http_parser)

if (WIN32)
	target_link_libraries(main Ws2_32)
endif()


