# BNES
[![CMake on multiple platforms](https://github.com/valerioformato/BNES/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/valerioformato/BNES/actions/workflows/cmake-multi-platform.yml)

BNES is a work-in-progress NES emulator written in modern C++23. It aims to provide a clean, modular, and testable
codebase for learning and experimenting with NES emulation.

## Features

- Written in C++23
- Modular structure (CPU, hardware, SDL-based rendering)
- Uses [SDL](https://www.libsdl.org/) for graphics and input
- Uses [magic_enum](https://github.com/Neargye/magic_enum) for modern enum utilities
- Optional unit tests with [Catch2](https://github.com/catchorg/Catch2)
- AddressSanitizer and ThreadSanitizer support for debugging

## Building

### Prerequisites

- CMake >= 3.16
- A C++23 compatible compiler (e.g., GCC 13+, Clang 16+, MSVC 2022+)
- SDL3 development libraries

### Build Instructions

```sh
git clone --recursive <this-repo-url>
cd NES
mkdir build && cd build
cmake ..
make
```

#### Optional Build Flags

- `-DASAN=ON` — Enable AddressSanitizer
- `-DTSAN=ON` — Enable ThreadSanitizer
- `-DENABLE_BNES_TESTS=ON` — Build unit tests

Example:

```sh
cmake -DASAN=ON -DENABLE_BNES_TESTS=ON ..
make
```

## Project Structure

- `src/` — Main emulator source code
    - `HW/` — NES hardware emulation (CPU, etc.)
    - `SDL/` — SDL-based rendering and window management
    - `common/` — Shared utilities and helpers
- `testsuite/` — Unit tests (requires Catch2)
- `vendor/` — Third-party dependencies (SDL, magic_enum, Catch2)

## Running

After building, the main emulator binary will be in `build/src/BNES`. Run it from the build directory:

```sh
./src/BNES
```

## Testing

If built with `-DENABLE_BNES_TESTS=ON`, run the tests:

```sh
./testsuite/run_tests
```

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
