cmake_minimum_required (VERSION 2.8.3) 
project (ode_num_int_package) 

set (CMAKE_CXX_STANDARD 11)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set (BUILD_SHARED_LIBS true)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(examples)
