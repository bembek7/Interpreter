cmake_minimum_required(VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("Interpreter")

# Specify C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the data path (adjust as needed)
set(EXAMPLE_CODES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ExampleCodes/")

# Configure the header file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/PathConfig.h.in"
    "${CMAKE_BINARY_DIR}/PathConfig.h"
)

include_directories("${CMAKE_BINARY_DIR}")

# Add a library target for sharing with the test executable
add_library(InterpreterLib "Lexer.cpp" "Lexer.h" "Position.h" "LexToken.cpp" "LexToken.h" "LexicalError.h" "LexicalError.cpp")

# Add the executable for running the program
add_executable(Interpreter "Main.cpp" "Position.h" "LexToken.cpp" "LexToken.h" "LexicalError.h" "LexicalError.cpp")

# Link the executable to the library
target_link_libraries(Interpreter PRIVATE InterpreterLib)

# Include FetchContent module for GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_subdirectory(Tests)
