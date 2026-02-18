# BNES
[![CMake on multiple platforms](https://github.com/valerioformato/BNES/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/valerioformato/BNES/actions/workflows/cmake-multi-platform.yml)

BNES is a work-in-progress NES emulator written in modern C++23. It aims to provide a clean, modular, and testable
codebase for learning and experimenting with NES emulation.

## Current Status

This project is in active development. Currently implemented:
- **6502 CPU**: Complete instruction set implementation with decode, execute, and disassembly support
  - Modular instruction organization with explicit template instantiation
  - Support for all official and undocumented opcodes
- **PPU (Picture Processing Unit)**: Basic PPU implementation with internal registers and color palette
- **Memory System**: Bus architecture with ROM loading capabilities
- **Debugging Tools**: CPU and PPU debuggers with instruction visualization and CHR ROM viewing
- **Test Suite**: Comprehensive unit tests and validation with nestest ROM

## Features

- Written in C++23 with modern practices
- Modular architecture (CPU, PPU, Bus, ROM loader, debugging tools)
- Cross-platform support (Linux, macOS, Windows with Clang)
- Uses [SDL3](https://www.libsdl.org/) for graphics and text rendering
- Uses [magic_enum](https://github.com/Neargye/magic_enum) for type-safe enum utilities
- Uses [spdlog](https://github.com/gabime/spdlog) for logging
- Uses [cxxopts](https://github.com/jarro2783/cxxopts) for command-line argument parsing
- Uses [range-v3](https://github.com/ericniebler/range-v3) for ranges utilities
- Comprehensive unit testing with [Catch2](https://github.com/catchorg/Catch2)
- CI/CD with GitHub Actions testing on GCC, Clang (Linux/macOS/Windows)
- AddressSanitizer and ThreadSanitizer support

## Building

### Prerequisites

- CMake >= 3.16
- A C++23 compatible compiler:
  - GCC 13+ (Linux)
  - Clang 16+ (Linux/macOS/Windows)
  - **Note**: MSVC is not supported due to use of GNU statement expressions
- Git (for cloning with submodules)

### Build Instructions

```sh
git clone --recursive https://github.com/valerioformato/BNES.git
cmake -S BNES -B BNES/build
cmake --build BNES/build
```

#### Optional Build Flags

- `-DASAN=ON` — Enable AddressSanitizer
- `-DTSAN=ON` — Enable ThreadSanitizer
- `-DENABLE_BNES_TESTS=ON` — Build unit tests (disabled by default)

Example:

```sh
cmake -S BNES -B BNES/build -DASAN=ON
cmake --build BNES/build
```

## Project Structure

- `src/` — Main emulator source code
    - `HW/` — NES hardware emulation (CPU, PPU, Bus, ROM loader)
        - `Instructions/` — Modular CPU instruction implementations
    - `SDLBind/` — SDL3-based rendering, text, and window management
    - `Tools/` — Development tools (CPU debugger, PPU debugger)
    - `common/` — Shared utilities and type helpers
- `testsuite/` — Test suites and validation
    - `unit/` — Unit tests with Catch2 (CPU, PPU, Bus, ROM)
    - `nestest/` — nestest ROM validation
    - `snake/` — Snake game test ROM
    - `text-rendering/` — SDL text rendering tests
- `vendor/` — Third-party dependencies (SDL3, SDL_ttf, magic_enum, Catch2, spdlog, cxxopts, range-v3)

## Running

After building, executables will be in the build directory:

```sh
# Main emulator (work in progress)
./BNES/build/src/BNES

# CPU debugger with snake ROM
./BNES/build/testsuite/snake/snake_rom

# nestest ROM validation
./BNES/build/testsuite/nestest/nestest
```

## Testing

Unit tests are built by default. To run them:

```sh
./BNES/build/testsuite/unit/run_unit_tests
```

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
