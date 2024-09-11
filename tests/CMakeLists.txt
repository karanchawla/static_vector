add_executable(test_static_vector test_static_vector.cpp)

# Link GoogleTest to the test executable
target_link_libraries(test_static_vector gtest gtest_main)

add_test(NAME StaticVectorTest COMMAND test_static_vector)