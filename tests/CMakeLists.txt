cmake_minimum_required (VERSION 3.1.3) 
project (ode_num_int_tests)

set (GTEST_DIR ../external/googletest/googletest)

include_directories(${GTEST_DIR}/include ${GTEST_DIR})

# aux_source_directory(. SOURCE_FILES)
file (GLOB_RECURSE SOURCE_FILES ./*.cpp)
add_executable(${PROJECT_NAME} ${GTEST_DIR}/src/gtest-all.cc ${GTEST_DIR}/src/gtest_main.cc ${SOURCE_FILES})

add_dependencies (${PROJECT_NAME} ode_num_int)
target_link_libraries (${PROJECT_NAME} ode_num_int)

set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY
  COMPILE_DEFINITIONS $<$<CONFIG:Debug>:_DEBUG>
)

if (UNIX)
    target_link_libraries (${PROJECT_NAME} pthread)
endif()
