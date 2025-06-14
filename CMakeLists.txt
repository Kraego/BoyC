cmake_minimum_required(VERSION 3.14)
project(MyCTestProject C CXX)

find_package(SDL2 QUIET)
if (SDL2_FOUND)
  message(STATUS "Using SDL2")
  set(DISPLAY_SRC src/display/display.cpp)
  set(DISPLAY_LIBS SDL2::SDL2)
else()
  message(STATUS "SDL2 not found - using stub display")
  set(DISPLAY_SRC src/display/display_stub.cpp)
  set(DISPLAY_LIBS)
endif()

# Configure include paths
include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/cpu
  ${CMAKE_SOURCE_DIR}/src/mem
  ${CMAKE_SOURCE_DIR}/src/rom
  ${CMAKE_SOURCE_DIR}/src/display
  ${CMAKE_SOURCE_DIR}/src/ppu
  ${SDL2_INCLUDE_DIRS}
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
  src/rom/rom.cpp
  src/display/display.cpp
  src/ppu/ppu.cpp)

target_link_libraries(boyc_exec ${DISPLAY_LIBS})

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
  tests/display/display-test.cpp
  tests/helper/test-helper.cpp
  src/cpu/cpu.cpp
  src/mem/mem.cpp
  src/rom/rom.cpp
  src/display/display.cpp
  src/ppu/ppu.cpp)

target_include_directories(tests PRIVATE
  tests/helper)

target_link_libraries(tests gtest_main ${DISPLAY_LIBS})
  
# Register the tests
include(GoogleTest)
gtest_discover_tests(tests)

# Get Test roms
include(ExternalProject)

ExternalProject_Add(
  gb_test_roms
  URL https://github.com/c-sp/game-boy-test-roms/releases/download/v7.0/game-boy-test-roms-v7.0.zip
  PREFIX ${CMAKE_BINARY_DIR}/gb_test_roms
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  LOG_DOWNLOAD ON
  DOWNLOAD_EXTRACT_TIMESTAMP ON
  )
  
# Provide the absolute path to the bundled test ROMs so unit tests can
# locate them when executed from the build directory.
target_compile_definitions(tests PRIVATE
  TEST_ROM_DIR="${CMAKE_BINARY_DIR}/gb_test_roms/src/gb_test_roms")
