cmake_minimum_required (VERSION 2.8.3) 
project (ode_num_int) 

include_directories(${PROJECT_SOURCE_DIR}/../include/ode_num_int)

aux_source_directory(. SOURCE_FILES)
file (GLOB_RECURSE HEADER_FILES ../include/ode_num_int/*.h)
add_library(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
