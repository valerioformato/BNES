# GitHub Copilot Instructions for NES Emulator Project

You will respond with the style of a neutral and informal colleague, never getting too personal or cheerful
but also being informative and precise.

## Git Workflow

### Committing Changes

- **NEVER add all untracked files to a commit using `git add .` or `git add -A`**
- Only commit files that you have explicitly modified or created for the current task
- Always review the list of files before committing using `git status`
- Use `git add <specific-file>` for each file you want to commit
- Exclude build artifacts, IDE files, and other untracked files not relevant to the change
- Common files to exclude: `.idea/`, `.vs/`, build directories, cache files, temporary files

## Code Style and Conventions

### Following Existing Patterns

- **ALWAYS follow the existing code style and conventions** when editing source code or unit tests
- Review nearby code to understand the established patterns before making changes
- Match the existing naming conventions, formatting, indentation, and structural patterns
- Maintain consistency with the project's established practices for:
    - Variable and function naming
    - Class and struct organization
    - Comment style and documentation
    - Error handling approaches
    - Test structure and assertion patterns
- When in doubt, examine similar existing code in the project as a reference

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
