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

# Build main executable only if SDL2 is available
if(SDL2_FOUND)
  add_executable(boyc_exec
    src/main.cpp
    src/mem/mem.cpp
    src/cpu/cpu.cpp
    src/rom/rom.cpp
    ${DISPLAY_SRC}
    src/ppu/ppu.cpp)

  target_link_libraries(boyc_exec ${DISPLAY_LIBS})
endif()


# Add test executable
add_executable(tests
  tests/main.c
  tests/ctest/ctest.c
  tests/cpu/cpu-test.cpp
  tests/display/display-test.cpp
  tests/helper/test-helper.cpp
  src/cpu/cpu.cpp
  src/mem/mem.cpp
  src/rom/rom.cpp
  ${DISPLAY_SRC}
  src/ppu/ppu.cpp)

target_include_directories(tests PRIVATE
  tests/helper
  tests/ctest)

target_link_libraries(tests ${DISPLAY_LIBS})
  
# Register the tests
add_test(NAME boyc_tests COMMAND tests)

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

# Get Test roms
set(TEST_ROM_DIR "")
target_compile_definitions(tests PRIVATE TEST_ROM_DIR="${TEST_ROM_DIR}")
