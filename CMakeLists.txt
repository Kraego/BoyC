cmake_minimum_required(VERSION 3.14)
project(MyCTestProject C CXX)

# Enable testing
enable_testing()

# Set C standard
set(CMAKE_C_STANDARD 11)

# Add your C source
add_executable(main_exec src/main.c)

# Download and add GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add test executable
add_executable(tests tests/cpu-test.c)
target_link_libraries(tests gtest_main)

# Register the tests
include(GoogleTest)
gtest_discover_tests(tests)
