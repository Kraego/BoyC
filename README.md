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

2. Install sdl (for graphics output / on arch)

   ```bash
   sudo pacman -Syu
   sudo pacman -S sdl2 sdl2_image sdl2_ttf libgl libglvnd mesa mesa-utils
   ```
   test it with: 
   ```bash
   glxinfo | grep OpenGL
   ```

3. Execute the test suite from the build directory:

   ```bash
   cd build && ctest --output-on-failure
   ```

   The unit tests rely on ROM images provided via cmake target: `GB_DOWNLOAD_TEST_ROMS`.

4. Run it:
   ```bash
   ./boyc_exec "gb_test_roms/src/gb_test_roms/blargg/cpu_instrs/cpu_instrs.gb"
   ```



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
   * opcodes site: https://meganesu.github.io/generate-gb-opcodes/
