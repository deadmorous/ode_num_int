cmake_minimum_required (VERSION 3.1.3) 
project (tests) 

include(../external/googletest)
add_executable(tests dummy_test.cpp)
gtest_add_tests(TARGET      ${PROJECT_NAME}
                TEST_SUFFIX .noArgs
                TEST_LIST   noArgsTests
)
# gtest_add_tests(TARGET      ${PROJECT_NAME}
#                 EXTRA_ARGS  --someArg someValue
#                 TEST_SUFFIX .withArgs
#                 TEST_LIST   withArgsTests
# )
set_tests_properties(${noArgsTests}   PROPERTIES TIMEOUT 10)
set_tests_properties(${withArgsTests} PROPERTIES TIMEOUT 20)

