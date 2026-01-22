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

## CPU Instruction Refactoring Strategy

### Overview
The CPU instructions in `src/HW/CPU.h` should be refactored using **explicit template instantiation** to move implementations from the header file to separate compilation units. This improves compile times and maintainability.

### Architecture

**Structure:**
```
src/HW/
├── CPU.h                                    (Declarations only)
├── CPU.cpp                                  (Core CPU logic)
├── Instructions/
│   ├── LoadStoreInstructions.cpp            (Load/Store implementations)
│   ├── ArithmeticInstructions.cpp           (Arithmetic implementations)
│   ├── LogicalInstructions.cpp              (Logical operations)
│   ├── ShiftRotateInstructions.cpp          (Shift/Rotate operations)
│   └── ...                                  (Other instruction categories)
```

### What Goes Where

#### In CPU.h (Header):
- Struct declarations with member variables
- Static constexpr methods (e.g., `AddrMode()`)
- Constructor/destructor declarations
- `Apply()` method declarations

**Example:**
```cpp
template <Register REG, AddressingMode MODE>
struct LoadRegister : DecodedInstruction {
  LoadRegister() = delete;
  explicit LoadRegister(uint16_t);  // Declaration only
  void Apply(CPU &cpu) const;       // Declaration only
  
  static constexpr AddressingMode AddrMode() { return MODE; }
  uint16_t value{0};
};
```

#### In .cpp Files (Implementation):
- Constructor implementations (including those with `if constexpr`)
- `Apply()` method implementations
- **Explicit template instantiations** for all used variants

**Example in LoadStoreInstructions.cpp:**
```cpp
// Constructor implementation
template <CPU::Register REG, AddressingMode MODE>
CPU::LoadRegister<REG, MODE>::LoadRegister(uint16_t _value) {
  this->size = 2;
  if constexpr (MODE == AddressingMode::Immediate) {
    this->cycles = 2;
  }
  // ... more if constexpr branches
  value = _value;
}

// Apply() implementation
template <CPU::Register REG, AddressingMode MODE>
void CPU::LoadRegister<REG, MODE>::Apply(CPU &cpu) const {
  // Implementation here
}

// CRITICAL: Explicit instantiations for ALL used variants
template struct CPU::LoadRegister<CPU::Register::A, AddressingMode::Immediate>;
template struct CPU::LoadRegister<CPU::Register::A, AddressingMode::Absolute>;
// ... all other variants used in the Instruction variant
```

### Key Principles

1. **Constructors with `if constexpr` CAN be moved to .cpp**
   - The `if constexpr` branches are resolved at instantiation time, not call time
   - Explicit instantiation compiles the entire struct including constructors

2. **Explicit Instantiation is Required**
   - Every template variant used in the `CPU::Instruction` variant MUST be explicitly instantiated
   - Missing instantiations will cause linker errors

3. **Include Order**
   - The .cpp files must include CPU.h
   - All necessary headers for the implementations (spdlog/fmt, magic_enum)

4. **CMakeLists.txt Updates**
   - Add each new .cpp file to the NESHW library target:
     ```cmake
     add_library(NESHW STATIC 
       CPU.cpp 
       Bus.cpp 
       Rom.cpp 
       Instructions/LoadStoreInstructions.cpp
       Instructions/ArithmeticInstructions.cpp
     )
     ```

### Instruction Categories

Group instructions logically:
- **LoadStore**: LoadRegister, StoreRegister
- **Arithmetic**: AddWithCarry, SubtractWithCarry, Increment, Decrement, IncrementRegister, DecrementRegister
- **Logical**: LogicalAND, ExclusiveOR, BitwiseOR, BitTest
- **ShiftRotate**: ShiftLeft, ShiftRight, RotateLeft, RotateRight
- **ControlFlow**: Branch, Jump, JumpToSubroutine, Return instructions
- **Compare**: CompareRegister
- **Transfer**: TransferRegisterTo, TransferStackPointerToX, TransferXToStackPointer
- **Stack**: Push/Pull operations
- **Flags**: ClearStatusFlag, SetStatusFlag
- **Special**: Break, NoOperation, DoubleNoOperation
- **Unofficial**: LAX, SAX, DCP, ISB, SLO, RLA, SRE, RRA, etc.

### Verification Steps

After refactoring each instruction:
1. Build the NESHW library: `cmake --build <build-dir> --target NESHW`
2. Verify no compilation errors
3. Run unit tests to ensure functionality is preserved
4. Commit with descriptive message about which instructions were refactored

### Benefits

- **Faster compile times**: Implementations compiled once, not in every translation unit
- **Smaller header files**: Easier to navigate and understand
- **Better organization**: Instructions grouped by category
- **Parallel compilation**: Multiple .cpp files can compile simultaneously
- **Maintainability**: Changes to implementations don't require recompiling everything that includes CPU.h
