//
// Created by vformato on 7/22/25.
//

#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

using namespace BNES::HW;

template <size_t N> using ByteArray = std::array<uint8_t, N>;

class CPUMock : public CPU {
  // This mock class is used to expose private methods for testing purposes.
public:
  using CPU::ReadFromMemory;
  using CPU::SetRegister;
  using CPU::SetStatusFlag;
  using CPU::StackPointer;
  using CPU::WriteToMemory;
};

SCENARIO("6502 instruction execution tests (all the rest)") {
  GIVEN("A freshly initialized cpu") {
    CPUMock cpu;
    auto original_program_counter = cpu.ProgramCounter();

    WHEN("We execute a BRK instruction") {
      THEN("It should throw a NonMaskableInterrupt exception") {
        REQUIRE_THROWS(cpu.RunInstruction(CPU::Break{}));
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }
    }

    WHEN("We execute a TAX instruction") {
      auto load_instr = CPU::LoadRegister<CPU::Register::A, AddressingMode::Immediate>{0x42};
      auto tax_instr = CPU::TransferRegisterTo<CPU::Register::A, CPU::Register::X>{};

      // First, load the accumulator with a value
      cpu.RunInstruction(load_instr);
      // Now execute the TAX instruction
      cpu.RunInstruction(tax_instr);

      THEN("The X register should be loaded with the value from the accumulator") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x42);
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      load_instr.value = 0x00; // LDA #$00
      cpu.RunInstruction(load_instr);
      cpu.RunInstruction(tax_instr);

      THEN("The X register should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute a TAY instruction") {
      auto load_instr = CPU::LoadRegister<CPU::Register::A, AddressingMode::Immediate>{0x42};
      auto tay_instr = CPU::TransferRegisterTo<CPU::Register::A, CPU::Register::Y>{};

      // First, load the accumulator with a value
      cpu.RunInstruction(load_instr);
      // Now execute the TAY instruction
      cpu.RunInstruction(tay_instr);

      THEN("The Y register should be loaded with the value from the accumulator") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x42);
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      load_instr.value = 0x00; // LDA #$00
      cpu.RunInstruction(load_instr);
      cpu.RunInstruction(tay_instr);

      THEN("The Y register should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute a TXA instruction") {
      auto load_x_instr = CPU::LoadRegister<CPU::Register::X, AddressingMode::Immediate>{0x42};
      auto txa_instr = CPU::TransferRegisterTo<CPU::Register::X, CPU::Register::A>{};

      // First, load the X register with a value
      cpu.RunInstruction(load_x_instr);
      // Now execute the TXA instruction
      cpu.RunInstruction(txa_instr);

      THEN("The accumulator should be loaded with the value from the X register") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x42);
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      load_x_instr.value = 0x00; // LDX #$00
      cpu.RunInstruction(load_x_instr);
      cpu.RunInstruction(txa_instr);

      THEN("The accumulator should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      load_x_instr.value = 0x80; // LDX #$80 (negative value)
      cpu.RunInstruction(load_x_instr);
      cpu.RunInstruction(txa_instr);

      THEN("The accumulator should have negative value and the negative flag should be set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x80);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a TYA instruction") {
      auto load_y_instr = CPU::LoadRegister<CPU::Register::Y, AddressingMode::Immediate>{0x42};
      auto tya_instr = CPU::TransferRegisterTo<CPU::Register::Y, CPU::Register::A>{};

      // First, load the Y register with a value
      cpu.RunInstruction(load_y_instr);
      // Now execute the TYA instruction
      cpu.RunInstruction(tya_instr);

      THEN("The accumulator should be loaded with the value from the Y register") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x42);
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      load_y_instr.value = 0x00; // LDY #$00
      cpu.RunInstruction(load_y_instr);
      cpu.RunInstruction(tya_instr);

      THEN("The accumulator should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      load_y_instr.value = 0x80; // LDY #$80 (negative value)
      cpu.RunInstruction(load_y_instr);
      cpu.RunInstruction(tya_instr);

      THEN("The accumulator should have negative value and the negative flag should be set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x80);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute CPX immediate instructions") {
      // Load X register with test value
      auto load_x = CPU::LoadRegister<CPU::Register::X, AddressingMode::Immediate>{0x42};
      cpu.RunInstruction(load_x);
      original_program_counter = cpu.ProgramCounter();

      // Test 1: X == operand (should set Zero and Carry flags)
      auto cpx_equal = CPU::CompareRegister<CPU::Register::X, AddressingMode::Immediate>{0x42};
      cpu.RunInstruction(cpx_equal);

      THEN("Zero and Carry flags should be set when X equals operand") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x42); // X register unchanged
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      // Test 2: X > operand (should set Carry flag, clear Zero)
      auto cpx_greater = CPU::CompareRegister<CPU::Register::X, AddressingMode::Immediate>{0x30};
      cpu.RunInstruction(cpx_greater);

      THEN("Carry flag should be set when X > operand") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x42); // X register unchanged
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      // Test 3: X < operand (should clear Carry flag, may set Negative)
      auto cpx_less = CPU::CompareRegister<CPU::Register::X, AddressingMode::Immediate>{0x50};
      cpu.RunInstruction(cpx_less);

      THEN("Carry flag should be clear when X < operand") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x42); // X register unchanged
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true); // 0x42 - 0x50 = 0xF2 (negative)
      }

      original_program_counter = cpu.ProgramCounter();

      // Test 4: Test with zero in X register
      auto load_x_zero = CPU::LoadRegister<CPU::Register::X, AddressingMode::Immediate>{0x00};
      cpu.RunInstruction(load_x_zero);
      auto cpx_zero = CPU::CompareRegister<CPU::Register::X, AddressingMode::Immediate>{0x01};
      cpu.RunInstruction(cpx_zero);

      THEN("Negative flag should be set when comparing 0 with positive number") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x00);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true); // 0x00 - 0x01 = 0xFF (negative)
      }
    }

    WHEN("We execute CPX zero-page instructions") {
      // Set up memory with test values
      cpu.WriteToMemory(0x50, 0x42);
      cpu.WriteToMemory(0x51, 0x30);
      cpu.WriteToMemory(0x52, 0x50);

      // Load X register with test value
      auto load_x = CPU::LoadRegister<CPU::Register::X, AddressingMode::Immediate>{0x42};
      cpu.RunInstruction(load_x);
      original_program_counter = cpu.ProgramCounter();

      // Test equality
      auto cpx_zp_equal = CPU::CompareRegister<CPU::Register::X, AddressingMode::ZeroPage>{0x50};
      cpu.RunInstruction(cpx_zp_equal);

      THEN("Zero and Carry flags should be set when X equals zero-page value") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute CPX absolute instructions") {
      // Set up memory with test value in valid RAM range
      cpu.WriteToMemory(0x0300, 0x42);

      // Load X register with test value
      auto load_x = CPU::LoadRegister<CPU::Register::X, AddressingMode::Immediate>{0x42};
      cpu.RunInstruction(load_x);
      original_program_counter = cpu.ProgramCounter();

      // Test equality
      auto cpx_abs_equal = CPU::CompareRegister<CPU::Register::X, AddressingMode::Absolute>{0x0300};
      cpu.RunInstruction(cpx_abs_equal);

      THEN("Zero and Carry flags should be set when X equals absolute value") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute CPY immediate instructions") {
      // Load Y register with test value
      auto load_y = CPU::LoadRegister<CPU::Register::Y, AddressingMode::Immediate>{0x42};
      cpu.RunInstruction(load_y);
      original_program_counter = cpu.ProgramCounter();

      // Test 1: Y == operand (should set Zero and Carry flags)
      auto cpy_equal = CPU::CompareRegister<CPU::Register::Y, AddressingMode::Immediate>{0x42};
      cpu.RunInstruction(cpy_equal);

      THEN("Zero and Carry flags should be set when Y equals operand") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x42); // Y register unchanged
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      // Test 2: Y > operand (should set Carry flag, clear Zero)
      auto cpy_greater = CPU::CompareRegister<CPU::Register::Y, AddressingMode::Immediate>{0x30};
      cpu.RunInstruction(cpy_greater);

      THEN("Carry flag should be set when Y > operand") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x42); // Y register unchanged
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      // Test 3: Y < operand (should clear Carry flag, may set Negative)
      auto cpy_less = CPU::CompareRegister<CPU::Register::Y, AddressingMode::Immediate>{0x50};
      cpu.RunInstruction(cpy_less);

      THEN("Carry flag should be clear when Y < operand") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x42); // Y register unchanged
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true); // 0x42 - 0x50 = 0xF2 (negative)
      }

      original_program_counter = cpu.ProgramCounter();

      // Test 4: Edge case with 0x80 (test negative flag behavior)
      auto load_y_80 = CPU::LoadRegister<CPU::Register::Y, AddressingMode::Immediate>{0x80};
      cpu.RunInstruction(load_y_80);
      auto cpy_80 = CPU::CompareRegister<CPU::Register::Y, AddressingMode::Immediate>{0x7F};
      cpu.RunInstruction(cpy_80);

      THEN("Should handle signed comparison correctly") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x80);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);     // 0x80 >= 0x7F
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false); // 0x80 - 0x7F = 0x01 (positive)
      }
    }

    WHEN("We execute CPY zero-page instructions") {
      // Set up memory with test values
      cpu.WriteToMemory(0x60, 0x42);
      cpu.WriteToMemory(0x61, 0x30);
      cpu.WriteToMemory(0x62, 0x50);

      // Load Y register with test value
      auto load_y = CPU::LoadRegister<CPU::Register::Y, AddressingMode::Immediate>{0x42};
      cpu.RunInstruction(load_y);
      original_program_counter = cpu.ProgramCounter();

      // Test equality
      auto cpy_zp_equal = CPU::CompareRegister<CPU::Register::Y, AddressingMode::ZeroPage>{0x60};
      cpu.RunInstruction(cpy_zp_equal);

      THEN("Zero and Carry flags should be set when Y equals zero-page value") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute CPY absolute instructions") {
      // Set up memory with test value in valid RAM range
      cpu.WriteToMemory(0x0400, 0x42);

      // Load Y register with test value
      auto load_y = CPU::LoadRegister<CPU::Register::Y, AddressingMode::Immediate>{0x42};
      cpu.RunInstruction(load_y);
      original_program_counter = cpu.ProgramCounter();

      // Test equality
      auto cpy_abs_equal = CPU::CompareRegister<CPU::Register::Y, AddressingMode::Absolute>{0x0400};
      cpu.RunInstruction(cpy_abs_equal);

      THEN("Zero and Carry flags should be set when Y equals absolute value") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute a BNE instruction") {
      auto bne = CPU::BranchIfNotEqual{0x04};

      cpu.SetStatusFlag(CPU::StatusFlag::Zero, true);
      cpu.RunInstruction(bne);
      THEN("The program counter should not change if Zero flag is set") {
        // Remember that the offset is relative to the byte after the current instruction
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlag(CPU::StatusFlag::Zero, false);
      cpu.RunInstruction(bne);
      THEN("The program counter should change if Zero flag is clear") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 6); // 2 bytes for BNE + 4 bytes for offset
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlag(CPU::StatusFlag::Zero, false);
      bne.offset = -0x5;
      cpu.RunInstruction(bne);
      THEN("The program counter should go back if Zero flag is clear and offset is negative") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter - 3); // 2 bytes for BNE - 5 byte for offset
      }
    }

    WHEN("We execute a BEQ instruction") {
      auto beq = CPU::BranchIfEqual{0x04};

      cpu.SetStatusFlag(CPU::StatusFlag::Zero, false);
      cpu.RunInstruction(beq);
      THEN("The program counter should not change if Zero flag is clear") {
        // Remember that the offset is relative to the byte after the current instruction
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlag(CPU::StatusFlag::Zero, true);
      cpu.RunInstruction(beq);
      THEN("The program counter should change if Zero flag is set") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 6); // 2 bytes for BEQ + 4 bytes for offset
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlag(CPU::StatusFlag::Zero, true);
      beq.offset = -0x5;
      cpu.RunInstruction(beq);
      THEN("The program counter should go back if Zero flag is set and offset is negative") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter - 3); // 2 bytes for BEQ - 5 byte for offset
      }
    }

    WHEN("We execute a JMP Absolute instruction") {
      auto jmp = CPU::Jump<AddressingMode::Absolute>{0x3000};

      cpu.RunInstruction(jmp);
      THEN("The program counter should jump to the target address") { REQUIRE(cpu.ProgramCounter() == 0x3000); }
    }

    WHEN("We execute a JMP Indirect instruction") {
      // First, set up the indirect address in memory
      // We'll store the target address 0x4000 at memory location 0x0200
      cpu.WriteToMemory(0x0200, 0x00); // Low byte of target address
      cpu.WriteToMemory(0x0201, 0x40); // High byte of target address

      auto jmp = CPU::Jump<AddressingMode::Indirect>{0x0200};

      cpu.RunInstruction(jmp);
      THEN("The program counter should jump to the address stored at the indirect location") {
        REQUIRE(cpu.ProgramCounter() == 0x4000);
      }
    }

    WHEN("We execute multiple JMP instructions") {
      auto jmp1 = CPU::Jump<AddressingMode::Absolute>{0x2000};
      auto jmp2 = CPU::Jump<AddressingMode::Absolute>{0x3000};

      cpu.RunInstruction(jmp1);
      THEN("First jump should work") { REQUIRE(cpu.ProgramCounter() == 0x2000); }

      cpu.RunInstruction(jmp2);
      THEN("Second jump should work") { REQUIRE(cpu.ProgramCounter() == 0x3000); }
    }

    WHEN("We execute a PHA instruction") {
      auto load_a_instr = CPU::LoadRegister<CPU::Register::A, AddressingMode::Immediate>{0x42};
      auto pha_instr = CPU::PushAccumulator{};

      // Store the initial stack pointer
      auto initial_stack_pointer = cpu.StackPointer();

      // First, load the accumulator with a value
      cpu.RunInstruction(load_a_instr);
      // Now execute the PHA instruction
      cpu.RunInstruction(pha_instr);

      THEN("The accumulator value should be pushed onto the stack") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x42);       // Accumulator unchanged
        REQUIRE(cpu.StackPointer() == initial_stack_pointer - 1); // Stack pointer decremented
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        // Check that the value was written to the stack (at stack base + old stack pointer)
        REQUIRE(cpu.ReadFromMemory(0x0100 + initial_stack_pointer) == 0x42);
      }

      original_program_counter = cpu.ProgramCounter();
      auto second_stack_pointer = cpu.StackPointer();

      // Test pushing multiple values
      load_a_instr.value = 0x80; // LDA #$80
      cpu.RunInstruction(load_a_instr);
      cpu.RunInstruction(pha_instr);

      THEN("Multiple values can be pushed onto the stack") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x80);
        REQUIRE(cpu.StackPointer() == second_stack_pointer - 1);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        // Check that the new value was written to the stack
        REQUIRE(cpu.ReadFromMemory(0x0100 + second_stack_pointer) == 0x80);
        // Verify the previous value is still there
        REQUIRE(cpu.ReadFromMemory(0x0100 + initial_stack_pointer) == 0x42);
      }
    }

    WHEN("We execute a PLA instruction") {
      auto load_a_instr = CPU::LoadRegister<CPU::Register::A, AddressingMode::Immediate>{0x42};
      auto pha_instr = CPU::PushAccumulator{};
      auto pla_instr = CPU::PullAccumulator{};

      // First, push a value onto the stack
      cpu.RunInstruction(load_a_instr);
      cpu.RunInstruction(pha_instr);
      auto stack_pointer_after_push = cpu.StackPointer();

      // Change the accumulator to a different value
      load_a_instr.value = 0x00; // LDA #$00
      cpu.RunInstruction(load_a_instr);
      original_program_counter = cpu.ProgramCounter();

      REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);

      // Now execute the PLA instruction
      cpu.RunInstruction(pla_instr);

      THEN("The value should be pulled from the stack into the accumulator") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x42);          // Value restored from stack
        REQUIRE(cpu.StackPointer() == stack_pointer_after_push + 1); // Stack pointer incremented
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }

      // Test pulling zero value and flag setting
      load_a_instr.value = 0x00;
      cpu.RunInstruction(load_a_instr);
      cpu.RunInstruction(pha_instr);
      original_program_counter = cpu.ProgramCounter();
      auto current_sp = cpu.StackPointer();
      cpu.RunInstruction(pla_instr);

      THEN("Pulling zero should set the zero flag") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.StackPointer() == uint8_t(current_sp + 1));
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      // Test pulling negative value and flag setting
      load_a_instr.value = 0x80;
      cpu.RunInstruction(load_a_instr);
      cpu.RunInstruction(pha_instr);
      original_program_counter = cpu.ProgramCounter();
      current_sp = cpu.StackPointer();
      cpu.RunInstruction(pla_instr);

      THEN("Pulling negative value should set the negative flag") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x80);
        REQUIRE(cpu.StackPointer() == uint8_t(current_sp + 1));
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute PHA and PLA in sequence (stack LIFO behavior)") {
      auto load_a_instr = CPU::LoadRegister<CPU::Register::A, AddressingMode::Immediate>{0x42};
      auto pha_instr = CPU::PushAccumulator{};
      auto pla_instr = CPU::PullAccumulator{};

      // Push first value
      load_a_instr.value = 0x11;
      cpu.RunInstruction(load_a_instr);
      cpu.RunInstruction(pha_instr);

      // Push second value
      load_a_instr.value = 0x22;
      cpu.RunInstruction(load_a_instr);
      cpu.RunInstruction(pha_instr);

      // Push third value
      load_a_instr.value = 0x33;
      cpu.RunInstruction(load_a_instr);
      cpu.RunInstruction(pha_instr);

      original_program_counter = cpu.ProgramCounter();

      // Pull values back - should be in reverse order (LIFO)
      cpu.RunInstruction(pla_instr);
      THEN("First pull should get the last pushed value") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x33);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.RunInstruction(pla_instr);
      THEN("Second pull should get the second-to-last pushed value") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.RunInstruction(pla_instr);
      THEN("Third pull should get the first pushed value") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x11);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }
    }
  }
}
