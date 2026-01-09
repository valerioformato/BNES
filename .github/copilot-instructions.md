# GitHub Copilot Instructions for NES Emulator Project

## Building and Testing

### Build Configuration
- Always leave 2 CPU cores available for the system when building
- Use `-j$(($(nproc) - 2))` instead of `-j$(nproc)` for parallel builds
- This ensures system responsiveness during compilation

### Unit Tests
- To build and run unit tests, always use the `run_unit_tests` target:
  ```bash
  cmake --build cmake-build-debug-gcc --target run_unit_tests -j$(($(nproc) - 2))
  ```
- Do not build individual test targets directly
