cmake_minimum_required(VERSION 3.14)
project(MyCTestProject C CXX)

# Allow building without downloading the external test ROMs.
option(GB_DOWNLOAD_TEST_ROMS "Download Game Boy test ROMs" OFF)

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

# Register the tests individually so IDEs can display each one
set(BOYC_TESTS
  cpu_dump_test_sueccess.cpu_dump
  cpu_reset_test_success.cpu_reset
  cpu_step_one_simple_step_success.cpu_step
  cpu_step_write_x8000_success.cpu_step
  cpu_step_oam_lock_success.cpu_step
  cpu_step_vram_unlocked_success.cpu_step
  cpu_step_ld_d_opcodes.cpu_step
  cpu_step_ld_a_hl.cpu_step
  cpu_step_additional_opcodes.cpu_step
  cpu_step_hl_memory_ops.cpu_step
  cpu_step_arithmetic_ops.cpu_step
  cpu_step_sp_and_hl_inc_dec_ops.cpu_step
  cpu_step_disable_interrupts.cpu_step
  cpu_step_xor_cp_ops.cpu_step
  cpu_step_adc_sbc_ops.cpu_step
  display_line_test.draw_line
  display_circle_test.draw_circle
)

foreach(test_name ${BOYC_TESTS})
  add_test(NAME ${test_name} COMMAND tests ${test_name})
endforeach()

# Optionally fetch the test ROMs when network access is available
if(GB_DOWNLOAD_TEST_ROMS)
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
endif()

# Get Test roms
set(TEST_ROM_DIR "")
target_compile_definitions(tests PRIVATE TEST_ROM_DIR="${TEST_ROM_DIR}")
