# Create a test executable
add_executable(InterpreterTest "Test.cpp")

# Link GoogleTest libraries
target_link_libraries(InterpreterTest GTest::gtest_main)

# Discover and register tests
include(GoogleTest)
gtest_discover_tests(InterpreterTest)