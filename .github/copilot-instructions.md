# GitHub Copilot Instructions for NES Emulator Project

## Building and Testing

### Build Configuration
- Always leave 2 CPU cores available for the system when building
- Use `-j$(($(nproc) - 2))` instead of `-j$(nproc)` for parallel builds on Linux/Unix
- This ensures system responsiveness during compilation

### Unit Tests

#### Linux/Unix (GCC)
- To build and run unit tests, always use the `run_unit_tests` target:
  ```bash
  cmake --build cmake-build-debug-gcc --target run_unit_tests -j$(($(nproc) - 2))
  ```
- Do not build individual test targets directly

#### Windows (Visual Studio/CLion)
- CMake may not be in PATH by default on Windows
- Use the CMake bundled with Visual Studio or CLion (search for cmake.exe in Program Files)
- Example with Visual Studio's CMake:
  ```powershell
  cmake --build cmake-build-debug-visual-studio --target run_unit_tests
  ```
- If cmake is not found, locate it first:
  ```powershell
  Get-ChildItem "C:\Program Files" -Filter cmake.exe -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
  ```
- After building, run tests manually from the build directory:
  ```powershell
  .\cmake-build-debug-visual-studio\testsuite\unit\run_unit_tests.exe
  ```
- To run specific test scenarios, use Catch2 filters:
  ```powershell
  .\cmake-build-debug-visual-studio\testsuite\unit\run_unit_tests.exe "[Execute]"
  ```
