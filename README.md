# BoyC

Game Boy Emulation in C (gcc9). For self study - NOT INDUSTRIAL GRADE MATERIAL

## Structure

* src ... sourcecode
* tests ... testcode
* doc ... documents

## Building and Running Tests

1. Configure and build the project:

   ```bash
   cmake -S . -B build
   cmake --build build
   ```

2. Execute the test suite from the build directory:

   ```bash
   cd build && ctest --output-on-failure
   ```

Using `ctest -T test` requires `DartConfiguration.tcl`, which is generated when
`include(CTest)` is present in the `CMakeLists.txt`. Running `ctest` as shown
above is sufficient for local testing.

## Todos

* [x] Check overview of GB
* [ ] Implement CPU emulation
* [ ] Emulate Memory and I/O
* [ ] Graphics
* [ ] Inputs
* [ ] Sound

## Links

* [Gameboy Development Wiki](https://gbdev.gg8.se/wiki/articles/Main_Page)
* CPU: https://www.chciken.com/tlmboy/2021/10/01/gameboy-cpu.html
