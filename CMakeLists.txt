cmake_minimum_required(VERSION 3.14)
project(MyCTestProject C CXX)

# Configure include paths
include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/cpu
  ${CMAKE_SOURCE_DIR}/src/mem
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
  src/cpu/cpu_ops.cpp)

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
add_executable(tests 
  tests/cpu/cpu-test.cpp
  src/cpu/cpu.cpp
  src/cpu/cpu_ops.cpp
  src/mem/mem.cpp)
  
target_link_libraries(tests gtest_main)

# Register the tests
include(GoogleTest)
gtest_discover_tests(tests)

# script to download binaries for test
set(DOWNLOAD_STAMP ${CMAKE_BINARY_DIR}/test_roms_downloaded.stamp)

add_custom_command(
    OUTPUT ${DOWNLOAD_STAMP}
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/download_test_roms.sh
    COMMAND ${CMAKE_COMMAND} -E touch ${DOWNLOAD_STAMP}
    COMMENT "Downloading test ROMs"
)

add_custom_target(download_roms ALL
    DEPENDS ${DOWNLOAD_STAMP}
)
