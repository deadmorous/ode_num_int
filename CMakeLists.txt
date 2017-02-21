cmake_minimum_required (VERSION 2.8.3) 
project (ode_num_int_package) 

set (CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/include)

# TODO: Remove
include_directories(${PROJECT_SOURCE_DIR}/include/ode_num_int)

add_subdirectory(src)
add_subdirectory(examples)
