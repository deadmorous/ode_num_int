cmake_minimum_required (VERSION 2.8.3) 
project (odexec) 

aux_source_directory(. SOURCE_FILE_LIST)
file (GLOB_RECURSE HEADER_FILES RELATIVE ${PROJECT_SOURCE_DIR} *.h *.hpp *.hxx)
add_executable(${PROJECT_NAME} ${SOURCE_FILE_LIST} ${HEADER_FILES})

add_dependencies (${PROJECT_NAME} ode_num_int)
target_link_libraries (${PROJECT_NAME} ode_num_int)

