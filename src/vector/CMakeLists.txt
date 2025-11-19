add_executable(vector_test vector_test.cpp)
target_include_directories(vector_test PRIVATE ${INCLUDES})
target_link_libraries(vector_test GTest::gtest_main)
