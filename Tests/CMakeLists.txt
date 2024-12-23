# Create a test executable
add_executable(InterpreterTest "LexerTest.cpp" "ParserTests.cpp" "ValueTests.cpp" "ParserTestsNewConvention.cpp" "InterpreterTests.cpp")

target_include_directories(InterpreterTest PRIVATE "${CMAKE_SOURCE_DIR}")

target_link_libraries(InterpreterTest PRIVATE InterpreterLib GTest::gtest_main)

# Discover and register tests
include(GoogleTest)
gtest_discover_tests(InterpreterTest)