cmake_minimum_required(VERSION 3.14)
project(MyCTestProject C CXX)

# Configure include paths
include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/cpu
  ${CMAKE_SOURCE_DIR}/src/mem
  ${CMAKE_SOURCE_DIR}/src/rom
)

# Enable testing
enable_testing()
include(CTest)

# Set C standard
set(CMAKE_C_STANDARD 11)

# Add your C source
add_executable(boyc_exec 
  src/main.cpp
  src/mem/mem.cpp
  src/cpu/cpu.cpp
  src/cpu/cpu_ops.cpp
  src/rom/rom.cpp)

# Download and add GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add test executable
add_executable(tests
  tests/cpu/cpu-test.cpp
  src/cpu/cpu.cpp
  src/cpu/cpu_ops.cpp
  src/mem/mem.cpp
  src/rom/rom.cpp)

# Provide the absolute path to the bundled test ROMs so unit tests can
# locate them when executed from the build directory.
target_compile_definitions(tests PRIVATE
  TEST_ROM_DIR="${CMAKE_SOURCE_DIR}/tests/test_roms/bin")
  
target_link_libraries(tests gtest_main)

# Register the tests
include(GoogleTest)
gtest_discover_tests(tests)
