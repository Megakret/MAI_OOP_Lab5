cmake_minimum_required(VERSION 3.10)
project(LabName)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Adding sanitizers and other flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=maybe-uninitialized -fsanitize=address")
#for clang to know about cmake
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#gtest installation
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)  
FetchContent_MakeAvailable(googletest)
#Adding tests
enable_testing()

#include headers here
set(INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/includes)
#target_include_directories(some_lib PRIVATE ${INCLUDES})
#link tests here
#target_link_libraries(tests lib_to_test GTest::gtest_main)
add_subdirectory(src/vector)
include(GoogleTest)
#gtest_discover_tests(tests)
