cmake_minimum_required (VERSION 3.1.3) 
project (ode_num_int_package) 

set (CMAKE_CXX_STANDARD 11)

option(BUILD_ODE_NUM_INT_TESTS "Build tests for ode_num_int" ON)
option(BUILD_ODE_NUM_INT_DOCS "Build documentation for ode_num_int" OFF)

if("${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" STREQUAL "")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
message(STATUS "${PROJECT_NAME} setting CMAKE_RUNTIME_OUTPUT_DIRECTORY to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()

if("${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" STREQUAL "")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
message(STATUS "${PROJECT_NAME} setting CMAKE_LIBRARY_OUTPUT_DIRECTORY to ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
endif()

if("${BUILD_SHARED_LIBS}" STREQUAL "")
set (BUILD_SHARED_LIBS true)
message(STATUS "${PROJECT_NAME} setting BUILD_SHARED_LIBS to true")
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(examples)

if (BUILD_ODE_NUM_INT_TESTS)
add_subdirectory(tests)
endif (BUILD_ODE_NUM_INT_TESTS)

if (BUILD_ODE_NUM_INT_DOCS)
    # check if Doxygen is installed
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT})
        message("Doxygen build started")

        # note the option ALL which allows to build the docs together with the application
        add_custom_target( docs ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
    else (DOXYGEN_FOUND)
      message("Doxygen needs to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
endif (BUILD_ODE_NUM_INT_DOCS)
