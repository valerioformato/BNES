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
  CPUMock(BNES::HW::Bus &bus) : CPU(bus) {}

  using CPU::ReadFromMemory;
  using CPU::SetRegister;
  using CPU::SetStatusFlagValue;
  using CPU::StackPointer;
  using CPU::WriteToMemory;
};

SCENARIO("6502 instruction execution tests (all the rest)", "[Execute]") {
  GIVEN("A freshly initialized cpu") {
    Bus bus;
    CPUMock cpu{bus};
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
      auto bne = CPU::Branch<Conditional::NotEqual>{0x04};

      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, true);
      cpu.RunInstruction(bne);
      THEN("The program counter should not change if Zero flag is set") {
        // Remember that the offset is relative to the byte after the current instruction
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, false);
      cpu.RunInstruction(bne);
      THEN("The program counter should change if Zero flag is clear") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 6); // 2 bytes for BNE + 4 bytes for offset
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, false);
      bne.offset = -0x5;
      cpu.RunInstruction(bne);
      THEN("The program counter should go back if Zero flag is clear and offset is negative") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter - 3); // 2 bytes for BNE - 5 byte for offset
      }
    }

    WHEN("We execute a BEQ instruction") {
      auto beq = CPU::Branch<Conditional::Equal>{0x04};

      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, false);
      cpu.RunInstruction(beq);
      THEN("The program counter should not change if Zero flag is clear") {
        // Remember that the offset is relative to the byte after the current instruction
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, true);
      cpu.RunInstruction(beq);
      THEN("The program counter should change if Zero flag is set") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 6); // 2 bytes for BEQ + 4 bytes for offset
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, true);
      beq.offset = -0x5;
      cpu.RunInstruction(beq);
      THEN("The program counter should go back if Zero flag is set and offset is negative") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter - 3); // 2 bytes for BEQ - 5 byte for offset
      }
    }

    WHEN("We execute a BCC instruction") {
      auto bcc = CPU::Branch<Conditional::CarryClear>{0x04};

      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.RunInstruction(bcc);
      THEN("The program counter should not change if Carry flag is set") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, false);
      cpu.RunInstruction(bcc);
      THEN("The program counter should change if Carry flag is clear") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 6); // 2 bytes for BCC + 4 bytes for offset
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, false);
      bcc.offset = -0x5;
      cpu.RunInstruction(bcc);
      THEN("The program counter should go back if Carry flag is clear and offset is negative") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter - 3); // 2 bytes for BCC - 5 byte for offset
      }
    }

    WHEN("We execute a BCS instruction") {
      auto bcs = CPU::Branch<Conditional::CarrySet>{0x04};

      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, false);
      cpu.RunInstruction(bcs);
      THEN("The program counter should not change if Carry flag is clear") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.RunInstruction(bcs);
      THEN("The program counter should change if Carry flag is set") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 6); // 2 bytes for BCS + 4 bytes for offset
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      bcs.offset = -0x5;
      cpu.RunInstruction(bcs);
      THEN("The program counter should go back if Carry flag is set and offset is negative") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter - 3); // 2 bytes for BCS - 5 byte for offset
      }
    }

    WHEN("We execute a BMI instruction") {
      auto bmi = CPU::Branch<Conditional::Minus>{0x04};

      cpu.SetStatusFlagValue(CPU::StatusFlag::Negative, false);
      cpu.RunInstruction(bmi);
      THEN("The program counter should not change if Negative flag is clear") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Negative, true);
      cpu.RunInstruction(bmi);
      THEN("The program counter should change if Negative flag is set") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 6); // 2 bytes for BMI + 4 bytes for offset
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Negative, true);
      bmi.offset = -0x5;
      cpu.RunInstruction(bmi);
      THEN("The program counter should go back if Negative flag is set and offset is negative") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter - 3); // 2 bytes for BMI - 5 byte for offset
      }
    }

    WHEN("We execute a BPL instruction") {
      auto bpl = CPU::Branch<Conditional::Positive>{0x04};

      cpu.SetStatusFlagValue(CPU::StatusFlag::Negative, true);
      cpu.RunInstruction(bpl);
      THEN("The program counter should not change if Negative flag is set") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Negative, false);
      cpu.RunInstruction(bpl);
      THEN("The program counter should change if Negative flag is clear") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 6); // 2 bytes for BPL + 4 bytes for offset
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Negative, false);
      bpl.offset = -0x5;
      cpu.RunInstruction(bpl);
      THEN("The program counter should go back if Negative flag is clear and offset is negative") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter - 3); // 2 bytes for BPL - 5 byte for offset
      }
    }

    WHEN("We execute a BVC instruction") {
      auto bvc = CPU::Branch<Conditional::OverflowClear>{0x04};

      cpu.SetStatusFlagValue(CPU::StatusFlag::Overflow, true);
      cpu.RunInstruction(bvc);
      THEN("The program counter should not change if Overflow flag is set") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Overflow, false);
      cpu.RunInstruction(bvc);
      THEN("The program counter should change if Overflow flag is clear") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 6); // 2 bytes for BVC + 4 bytes for offset
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Overflow, false);
      bvc.offset = -0x5;
      cpu.RunInstruction(bvc);
      THEN("The program counter should go back if Overflow flag is clear and offset is negative") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter - 3); // 2 bytes for BVC - 5 byte for offset
      }
    }

    WHEN("We execute a BVS instruction") {
      auto bvs = CPU::Branch<Conditional::OverflowSet>{0x04};

      cpu.SetStatusFlagValue(CPU::StatusFlag::Overflow, false);
      cpu.RunInstruction(bvs);
      THEN("The program counter should not change if Overflow flag is clear") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Overflow, true);
      cpu.RunInstruction(bvs);
      THEN("The program counter should change if Overflow flag is set") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 6); // 2 bytes for BVS + 4 bytes for offset
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Overflow, true);
      bvs.offset = -0x5;
      cpu.RunInstruction(bvs);
      THEN("The program counter should go back if Overflow flag is set and offset is negative") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter - 3); // 2 bytes for BVS - 5 byte for offset
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

    WHEN("We execute a PHP instruction") {
      auto php_instr = CPU::PushStatusRegister{};

      // Set up some status flags
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, true);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Negative, false);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Overflow, true);

      auto initial_stack_pointer = cpu.StackPointer();
      auto initial_status = cpu.StatusFlags();

      // Execute the PHP instruction
      cpu.RunInstruction(php_instr);

      THEN("The status register should be pushed onto the stack") {
        REQUIRE(cpu.StatusFlags() == initial_status);             // Status flags unchanged
        REQUIRE(cpu.StackPointer() == initial_stack_pointer - 1); // Stack pointer decremented
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        // Check that the status value was written to the stack
        // Note: PHP sets bits 4 and 5 (Break and unused) when pushing
        uint8_t expected_status = static_cast<uint8_t>(initial_status.to_ulong()) | 0x30;
        REQUIRE(cpu.ReadFromMemory(0x0100 + initial_stack_pointer) == expected_status);
      }

      original_program_counter = cpu.ProgramCounter();
      auto second_stack_pointer = cpu.StackPointer();

      // Change some flags and push again
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, false);
      cpu.SetStatusFlagValue(CPU::StatusFlag::InterruptDisable, true);
      auto second_status = cpu.StatusFlags();
      cpu.RunInstruction(php_instr);

      THEN("Multiple status values can be pushed onto the stack") {
        REQUIRE(cpu.StatusFlags() == second_status);
        REQUIRE(cpu.StackPointer() == second_stack_pointer - 1);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        // Check that the new value was written to the stack
        uint8_t expected_status = static_cast<uint8_t>(second_status.to_ulong()) | 0x30;
        REQUIRE(cpu.ReadFromMemory(0x0100 + second_stack_pointer) == expected_status);
      }
    }

    WHEN("We execute a PLP instruction") {
      auto php_instr = CPU::PushStatusRegister{};
      auto plp_instr = CPU::PullStatusRegister{};

      // Set up initial status flags
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, true);
      cpu.SetStatusFlagValue(CPU::StatusFlag::InterruptDisable, false);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Negative, true);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Overflow, false);

      // Push the status register onto the stack
      cpu.RunInstruction(php_instr);
      auto stack_pointer_after_push = cpu.StackPointer();

      // Change the status flags to different values
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, false);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, false);
      cpu.SetStatusFlagValue(CPU::StatusFlag::InterruptDisable, true);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Negative, false);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Overflow, true);

      original_program_counter = cpu.ProgramCounter();

      // Now execute the PLP instruction
      cpu.RunInstruction(plp_instr);

      THEN("The status register should be pulled from the stack") {
        REQUIRE(cpu.StackPointer() == stack_pointer_after_push + 1); // Stack pointer incremented
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        // Check that status flags match the original (ignoring Break and unused flags)
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::InterruptDisable) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }

      // Test pulling different status values
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, false);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, false);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Negative, false);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Overflow, true);
      cpu.SetStatusFlagValue(CPU::StatusFlag::InterruptDisable, true);

      cpu.RunInstruction(php_instr);
      original_program_counter = cpu.ProgramCounter();
      auto current_sp = cpu.StackPointer();

      // Change flags again
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, true);

      cpu.RunInstruction(plp_instr);

      THEN("Different status values can be pulled correctly") {
        REQUIRE(cpu.StackPointer() == uint8_t(current_sp + 1));
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::InterruptDisable) == true);
      }
    }

    WHEN("We execute PHP and PLP in sequence (stack LIFO behavior)") {
      auto php_instr = CPU::PushStatusRegister{};
      auto plp_instr = CPU::PullStatusRegister{};

      // Push first status state
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, false);
      cpu.RunInstruction(php_instr);

      // Push second status state
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, false);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, true);
      cpu.RunInstruction(php_instr);

      // Push third status state
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, true);
      cpu.RunInstruction(php_instr);

      original_program_counter = cpu.ProgramCounter();

      // Change flags to something else
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, false);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, false);

      // Pull values back - should be in reverse order (LIFO)
      cpu.RunInstruction(plp_instr);
      THEN("First pull should restore the last pushed status") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.RunInstruction(plp_instr);
      THEN("Second pull should restore the second-to-last pushed status") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.RunInstruction(plp_instr);
      THEN("Third pull should restore the first pushed status") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }
    }

    WHEN("We execute a TSX instruction") {
      auto tsx_instr = CPU::TransferStackPointerToX{};

      // Stack pointer should be at 0xFD initially
      auto initial_sp = cpu.StackPointer();

      // Execute TSX
      cpu.RunInstruction(tsx_instr);

      THEN("X register should contain the stack pointer value and flags should be set correctly") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == initial_sp);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true); // 0xFD has bit 7 set
      }

      original_program_counter = cpu.ProgramCounter();

      // Push some values to change stack pointer
      auto pha_instr = CPU::PushAccumulator{};
      auto load_a = CPU::LoadRegister<CPU::Register::A, AddressingMode::Immediate>{0x42};
      cpu.RunInstruction(load_a);
      cpu.RunInstruction(pha_instr); // SP now 0xFC (pushed from 0xFD)
      cpu.RunInstruction(pha_instr); // SP now 0xFB (pushed from 0xFC)
      cpu.RunInstruction(pha_instr); // SP now 0xFA (pushed from 0xFB)

      original_program_counter = cpu.ProgramCounter();

      // Execute TSX again
      cpu.RunInstruction(tsx_instr);

      THEN("X register should contain the updated stack pointer") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0xFA);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true); // 0xFA has bit 7 set
      }
    }

    WHEN("We execute a TXS instruction") {
      auto txs_instr = CPU::TransferXToStackPointer{};
      auto load_x = CPU::LoadRegister<CPU::Register::X, AddressingMode::Immediate>{0x80};

      // Load X with a specific value
      cpu.RunInstruction(load_x);
      original_program_counter = cpu.ProgramCounter();

      // Execute TXS
      cpu.RunInstruction(txs_instr);

      THEN("Stack pointer should be set to X register value and flags should not change") {
        REQUIRE(cpu.StackPointer() == 0x80);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        // TXS does not affect status flags
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true); // From LDX #$80
      }

      original_program_counter = cpu.ProgramCounter();

      // Load X with zero and transfer
      load_x.value = 0x00;
      cpu.RunInstruction(load_x);
      original_program_counter = cpu.ProgramCounter();
      cpu.RunInstruction(txs_instr);

      THEN("Stack pointer should be set to zero") {
        REQUIRE(cpu.StackPointer() == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        // TXS does not affect status flags (zero flag should be set by LDX, not TXS)
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute a BIT zero-page instructions") {
      // Set up memory with test values
      cpu.WriteToMemory(0x50, 0xC0); // 11000000 - bits 7 and 6 set (Negative and Overflow)
      cpu.WriteToMemory(0x51, 0x80); // 10000000 - bit 7 set (Negative only)
      cpu.WriteToMemory(0x52, 0x40); // 01000000 - bit 6 set (Overflow only)
      cpu.WriteToMemory(0x53, 0x00); // 00000000 - no bits set

      // Test 1: BIT with accumulator = 0xFF, memory = 0xC0 (should set N, V, clear Z)
      auto load_a = CPU::LoadRegister<CPU::Register::A, AddressingMode::Immediate>{0xFF};
      cpu.RunInstruction(load_a);
      original_program_counter = cpu.ProgramCounter();

      auto bit_test1 = CPU::BitTest<AddressingMode::ZeroPage>{0x50};
      cpu.RunInstruction(bit_test1);

      THEN("BIT should set Negative and Overflow flags, clear Zero flag") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFF); // Accumulator unchanged
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);    // 0xFF & 0xC0 = 0xC0 (non-zero)
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true); // Bit 7 from memory (0xC0)
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == true); // Bit 6 from memory (0xC0)
      }

      original_program_counter = cpu.ProgramCounter();

      // Test 2: BIT with accumulator = 0x00, memory = 0xC0 (should set Z, N, V)
      load_a.value = 0x00;
      cpu.RunInstruction(load_a);
      cpu.RunInstruction(bit_test1);

      THEN("BIT should set Zero, Negative, and Overflow flags") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00); // Accumulator unchanged
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 4);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);     // 0x00 & 0xC0 = 0x00 (zero)
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true); // Bit 7 from memory (0xC0)
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == true); // Bit 6 from memory (0xC0)
      }

      original_program_counter = cpu.ProgramCounter();

      // Test 3: BIT with memory = 0x80 (Negative only)
      load_a.value = 0xFF;
      cpu.RunInstruction(load_a);
      auto bit_test2 = CPU::BitTest<AddressingMode::ZeroPage>{0x51};
      cpu.RunInstruction(bit_test2);

      THEN("BIT should set Negative flag only") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFF); // Accumulator unchanged
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 4);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);     // 0xFF & 0x80 = 0x80 (non-zero)
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);  // Bit 7 from memory (0x80)
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false); // Bit 6 from memory (0x80)
      }

      original_program_counter = cpu.ProgramCounter();

      // Test 4: BIT with memory = 0x40 (Overflow only)
      auto bit_test3 = CPU::BitTest<AddressingMode::ZeroPage>{0x52};
      cpu.RunInstruction(bit_test3);

      THEN("BIT should set Overflow flag only") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFF); // Accumulator unchanged
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);     // 0xFF & 0x40 = 0x40 (non-zero)
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false); // Bit 7 from memory (0x40)
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == true);  // Bit 6 from memory (0x40)
      }

      original_program_counter = cpu.ProgramCounter();

      // Test 5: BIT with memory = 0x00 (no flags from memory)
      auto bit_test4 = CPU::BitTest<AddressingMode::ZeroPage>{0x53};
      cpu.RunInstruction(bit_test4);

      THEN("BIT should clear Negative and Overflow flags, set Zero flag") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFF); // Accumulator unchanged
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);      // 0xFF & 0x00 = 0x00 (zero)
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false); // Bit 7 from memory (0x00)
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false); // Bit 6 from memory (0x00)
      }
    }

    WHEN("We execute BIT absolute instructions") {
      // Set up memory with test values in valid RAM range
      cpu.WriteToMemory(0x0500, 0xC0); // 11000000 - bits 7 and 6 set
      cpu.WriteToMemory(0x0501, 0x55); // 01010101 - mixed pattern

      // Test 1: BIT absolute with full pattern
      auto load_a = CPU::LoadRegister<CPU::Register::A, AddressingMode::Immediate>{0xAA}; // 10101010
      cpu.RunInstruction(load_a);
      original_program_counter = cpu.ProgramCounter();

      auto bit_abs_test1 = CPU::BitTest<AddressingMode::Absolute>{0x0500};
      cpu.RunInstruction(bit_abs_test1);

      THEN("BIT absolute should work correctly") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xAA); // Accumulator unchanged
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);    // 0xAA & 0xC0 = 0x80 (non-zero)
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true); // Bit 7 from memory (0xC0)
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == true); // Bit 6 from memory (0xC0)
      }

      original_program_counter = cpu.ProgramCounter();

      // Test 2: BIT absolute with different pattern
      auto bit_abs_test2 = CPU::BitTest<AddressingMode::Absolute>{0x0501};
      cpu.RunInstruction(bit_abs_test2);

      THEN("BIT absolute should handle different memory patterns") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xAA); // Accumulator unchanged
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);      // 0xAA & 0x55 = 0x00 (zero)
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false); // Bit 7 from memory (0x55)
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == true);  // Bit 6 from memory (0x55)
      }

      original_program_counter = cpu.ProgramCounter();

      // Test 3: BIT with accumulator = 0xFF (should reveal all bits)
      load_a.value = 0xFF;
      cpu.RunInstruction(load_a);
      cpu.RunInstruction(bit_abs_test2);

      THEN("BIT with 0xFF accumulator should reveal memory pattern") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFF); // Accumulator unchanged
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 5);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);     // 0xFF & 0x55 = 0x55 (non-zero)
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false); // Bit 7 from memory (0x55)
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == true);  // Bit 6 from memory (0x55)
      }
    }

    WHEN("We execute a BNE instruction") {
      auto bne = CPU::Branch<Conditional::NotEqual>{0x04};

      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, true);
      cpu.RunInstruction(bne);
      THEN("The program counter should not change if Zero flag is set") {
        // Remember that the offset is relative to the byte after the current instruction
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, false);
      cpu.RunInstruction(bne);
      THEN("The program counter should change if Zero flag is clear") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 6); // 2 bytes for BNE + 4 bytes for offset
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, false);
      bne.offset = -0x5;
      cpu.RunInstruction(bne);
      THEN("The program counter should go back if Zero flag is clear and offset is negative") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter - 3); // 2 bytes for BNE - 5 byte for offset
      }
    }

    WHEN("We execute a BEQ instruction") {
      auto beq = CPU::Branch<Conditional::Equal>{0x04};

      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, false);
      cpu.RunInstruction(beq);
      THEN("The program counter should not change if Zero flag is clear") {
        // Remember that the offset is relative to the byte after the current instruction
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, true);
      cpu.RunInstruction(beq);
      THEN("The program counter should change if Zero flag is set") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 6); // 2 bytes for BEQ + 4 bytes for offset
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Zero, true);
      beq.offset = -0x5;
      cpu.RunInstruction(beq);
      THEN("The program counter should go back if Zero flag is set and offset is negative") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter - 3); // 2 bytes for BEQ - 5 byte for offset
      }
    }

    WHEN("We execute a BCC instruction") {
      auto bcc = CPU::Branch<Conditional::CarryClear>{0x04};

      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.RunInstruction(bcc);
      THEN("The program counter should not change if Carry flag is set") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, false);
      cpu.RunInstruction(bcc);
      THEN("The program counter should change if Carry flag is clear") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 6); // 2 bytes for BCC + 4 bytes for offset
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, false);
      bcc.offset = -0x5;
      cpu.RunInstruction(bcc);
      THEN("The program counter should go back if Carry flag is clear and offset is negative") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter - 3); // 2 bytes for BCC - 5 byte for offset
      }
    }

    WHEN("We execute a BCS instruction") {
      auto bcs = CPU::Branch<Conditional::CarrySet>{0x04};

      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, false);
      cpu.RunInstruction(bcs);
      THEN("The program counter should not change if Carry flag is clear") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.RunInstruction(bcs);
      THEN("The program counter should change if Carry flag is set") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 6); // 2 bytes for BCS + 4 bytes for offset
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      bcs.offset = -0x5;
      cpu.RunInstruction(bcs);
      THEN("The program counter should go back if Carry flag is set and offset is negative") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter - 3); // 2 bytes for BCS - 5 byte for offset
      }
    }

    WHEN("We execute a BMI instruction") {
      auto bmi = CPU::Branch<Conditional::Minus>{0x04};

      cpu.SetStatusFlagValue(CPU::StatusFlag::Negative, false);
      cpu.RunInstruction(bmi);
      THEN("The program counter should not change if Negative flag is clear") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Negative, true);
      cpu.RunInstruction(bmi);
      THEN("The program counter should change if Negative flag is set") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 6); // 2 bytes for BMI + 4 bytes for offset
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Negative, true);
      bmi.offset = -0x5;
      cpu.RunInstruction(bmi);
      THEN("The program counter should go back if Negative flag is set and offset is negative") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter - 3); // 2 bytes for BMI - 5 byte for offset
      }
    }

    WHEN("We execute a BPL instruction") {
      auto bpl = CPU::Branch<Conditional::Positive>{0x04};

      cpu.SetStatusFlagValue(CPU::StatusFlag::Negative, true);
      cpu.RunInstruction(bpl);
      THEN("The program counter should not change if Negative flag is set") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Negative, false);
      cpu.RunInstruction(bpl);
      THEN("The program counter should change if Negative flag is clear") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 6); // 2 bytes for BPL + 4 bytes for offset
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Negative, false);
      bpl.offset = -0x5;
      cpu.RunInstruction(bpl);
      THEN("The program counter should go back if Negative flag is clear and offset is negative") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter - 3); // 2 bytes for BPL - 5 byte for offset
      }
    }

    WHEN("We execute a BVC instruction") {
      auto bvc = CPU::Branch<Conditional::OverflowClear>{0x04};

      cpu.SetStatusFlagValue(CPU::StatusFlag::Overflow, true);
      cpu.RunInstruction(bvc);
      THEN("The program counter should not change if Overflow flag is set") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Overflow, false);
      cpu.RunInstruction(bvc);
      THEN("The program counter should change if Overflow flag is clear") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 6); // 2 bytes for BVC + 4 bytes for offset
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Overflow, false);
      bvc.offset = -0x5;
      cpu.RunInstruction(bvc);
      THEN("The program counter should go back if Overflow flag is clear and offset is negative") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter - 3); // 2 bytes for BVC - 5 byte for offset
      }
    }

    WHEN("We execute a BVS instruction") {
      auto bvs = CPU::Branch<Conditional::OverflowSet>{0x04};

      cpu.SetStatusFlagValue(CPU::StatusFlag::Overflow, false);
      cpu.RunInstruction(bvs);
      THEN("The program counter should not change if Overflow flag is clear") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Overflow, true);
      cpu.RunInstruction(bvs);
      THEN("The program counter should change if Overflow flag is set") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 6); // 2 bytes for BVS + 4 bytes for offset
      }

      original_program_counter = cpu.ProgramCounter();
      cpu.SetStatusFlagValue(CPU::StatusFlag::Overflow, true);
      bvs.offset = -0x5;
      cpu.RunInstruction(bvs);
      THEN("The program counter should go back if Overflow flag is set and offset is negative") {
        REQUIRE(cpu.ProgramCounter() == original_program_counter - 3); // 2 bytes for BVS - 5 byte for offset
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

    WHEN("We execute a CLC instruction") {
      // Set the carry flag first to test clearing
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      original_program_counter = cpu.ProgramCounter();

      auto clc_instr = CPU::ClearStatusFlag<CPU::StatusFlag::Carry>{};
      cpu.RunInstruction(clc_instr);

      THEN("The carry flag should be cleared") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }

      // Test that it doesn't affect the flag if already clear
      original_program_counter = cpu.ProgramCounter();
      cpu.RunInstruction(clc_instr);

      THEN("The carry flag should remain clear") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }
    }

    WHEN("We execute a CLD instruction") {
      // Set the decimal mode flag first to test clearing
      cpu.SetStatusFlagValue(CPU::StatusFlag::DecimalMode, true);
      original_program_counter = cpu.ProgramCounter();

      auto cld_instr = CPU::ClearStatusFlag<CPU::StatusFlag::DecimalMode>{};
      cpu.RunInstruction(cld_instr);

      THEN("The decimal mode flag should be cleared") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::DecimalMode) == false);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }

      // Test that it doesn't affect the flag if already clear
      original_program_counter = cpu.ProgramCounter();
      cpu.RunInstruction(cld_instr);

      THEN("The decimal mode flag should remain clear") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::DecimalMode) == false);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }
    }

    WHEN("We execute a CLI instruction") {
      // Set the interrupt disable flag first to test clearing
      cpu.SetStatusFlagValue(CPU::StatusFlag::InterruptDisable, true);
      original_program_counter = cpu.ProgramCounter();

      auto cli_instr = CPU::ClearStatusFlag<CPU::StatusFlag::InterruptDisable>{};
      cpu.RunInstruction(cli_instr);

      THEN("The interrupt disable flag should be cleared") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::InterruptDisable) == false);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }

      // Test that it doesn't affect the flag if already clear
      original_program_counter = cpu.ProgramCounter();
      cpu.RunInstruction(cli_instr);

      THEN("The interrupt disable flag should remain clear") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::InterruptDisable) == false);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }
    }

    WHEN("We execute a CLV instruction") {
      // Set the overflow flag first to test clearing
      cpu.SetStatusFlagValue(CPU::StatusFlag::Overflow, true);
      original_program_counter = cpu.ProgramCounter();

      auto clv_instr = CPU::ClearStatusFlag<CPU::StatusFlag::Overflow>{};
      cpu.RunInstruction(clv_instr);

      THEN("The overflow flag should be cleared") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }

      // Test that it doesn't affect the flag if already clear
      original_program_counter = cpu.ProgramCounter();
      cpu.RunInstruction(clv_instr);

      THEN("The overflow flag should remain clear") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }
    }

    WHEN("We execute a SEC instruction") {
      // Clear the carry flag first to test setting
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, false);
      original_program_counter = cpu.ProgramCounter();

      auto sec_instr = CPU::SetStatusFlag<CPU::StatusFlag::Carry>{};
      cpu.RunInstruction(sec_instr);

      THEN("The carry flag should be set") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }

      // Test that it doesn't affect the flag if already set
      original_program_counter = cpu.ProgramCounter();
      cpu.RunInstruction(sec_instr);

      THEN("The carry flag should remain set") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }
    }

    WHEN("We execute a SED instruction") {
      // Clear the decimal mode flag first to test setting
      cpu.SetStatusFlagValue(CPU::StatusFlag::DecimalMode, false);
      original_program_counter = cpu.ProgramCounter();

      auto sed_instr = CPU::SetStatusFlag<CPU::StatusFlag::DecimalMode>{};
      cpu.RunInstruction(sed_instr);

      THEN("The decimal mode flag should be set") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::DecimalMode) == true);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }

      // Test that it doesn't affect the flag if already set
      original_program_counter = cpu.ProgramCounter();
      cpu.RunInstruction(sed_instr);

      THEN("The decimal mode flag should remain set") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::DecimalMode) == true);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }
    }

    WHEN("We execute a SEI instruction") {
      // Clear the interrupt disable flag first to test setting
      cpu.SetStatusFlagValue(CPU::StatusFlag::InterruptDisable, false);
      original_program_counter = cpu.ProgramCounter();

      auto sei_instr = CPU::SetStatusFlag<CPU::StatusFlag::InterruptDisable>{};
      cpu.RunInstruction(sei_instr);

      THEN("The interrupt disable flag should be set") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::InterruptDisable) == true);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }

      // Test that it doesn't affect the flag if already set
      original_program_counter = cpu.ProgramCounter();
      cpu.RunInstruction(sei_instr);

      THEN("The interrupt disable flag should remain set") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::InterruptDisable) == true);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
      }
    }

    WHEN("We execute flag manipulation instructions in sequence") {
      // Test that flag instructions don't interfere with each other
      auto clc_instr = CPU::ClearStatusFlag<CPU::StatusFlag::Carry>{};
      auto sec_instr = CPU::SetStatusFlag<CPU::StatusFlag::Carry>{};
      auto cli_instr = CPU::ClearStatusFlag<CPU::StatusFlag::InterruptDisable>{};
      auto sei_instr = CPU::SetStatusFlag<CPU::StatusFlag::InterruptDisable>{};

      // Set up original state
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.SetStatusFlagValue(CPU::StatusFlag::InterruptDisable, true);
      original_program_counter = cpu.ProgramCounter();

      // Clear carry, clear interrupt disable
      cpu.RunInstruction(clc_instr);
      cpu.RunInstruction(cli_instr);

      THEN("Both flags should be cleared independently") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::InterruptDisable) == false);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }

      original_program_counter = cpu.ProgramCounter();

      // Set carry, set interrupt disable
      cpu.RunInstruction(sec_instr);
      cpu.RunInstruction(sei_instr);

      THEN("Both flags should be set independently") {
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::InterruptDisable) == true);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
    }

    WHEN("We execute a JSR instruction") {
      // Store initial state
      auto initial_stack_pointer = cpu.StackPointer();
      original_program_counter = cpu.ProgramCounter();

      // Test basic JSR functionality
      auto jsr = CPU::JumpToSubroutine{0x4000};
      cpu.RunInstruction(jsr);

      THEN("The program counter should jump to the target address and return address should be pushed to stack") {
        REQUIRE(cpu.ProgramCounter() == 0x4000);
        REQUIRE(cpu.StackPointer() == initial_stack_pointer - 2); // Stack pointer decremented by 2

        // Calculate expected return address (PC + instruction size - 1)
        uint16_t expected_return_address = original_program_counter + 3 - 1;

        // Check that return address was pushed to stack correctly (high byte first, then low byte)
        REQUIRE(cpu.ReadFromMemory(0x0100 + initial_stack_pointer) ==
                ((expected_return_address >> 8) & 0xFF)); // High byte
        REQUIRE(cpu.ReadFromMemory(0x0100 + initial_stack_pointer - 1) == (expected_return_address & 0xFF)); // Low byte
      }
    }

    WHEN("We execute multiple JSR instructions (nested subroutines)") {
      auto initial_stack_pointer = cpu.StackPointer();
      original_program_counter = cpu.ProgramCounter();

      // First JSR
      auto jsr1 = CPU::JumpToSubroutine{0x3000};
      cpu.RunInstruction(jsr1);

      auto first_return_address = original_program_counter + 3 - 1;
      auto stack_pointer_after_first = cpu.StackPointer();

      THEN("First JSR should work correctly") {
        REQUIRE(cpu.ProgramCounter() == 0x3000);
        REQUIRE(cpu.StackPointer() == initial_stack_pointer - 2);
      }

      // Second JSR (simulating a nested call)
      auto jsr2 = CPU::JumpToSubroutine{0x5000};
      cpu.RunInstruction(jsr2);

      auto second_return_address = 0x3000 + 3 - 1; // Return to address after second JSR

      THEN("Nested JSR should work correctly") {
        REQUIRE(cpu.ProgramCounter() == 0x5000);
        REQUIRE(cpu.StackPointer() == initial_stack_pointer - 4); // Two return addresses on stack

        // Check first return address (should still be there)
        REQUIRE(cpu.ReadFromMemory(0x0100 + initial_stack_pointer) == ((first_return_address >> 8) & 0xFF));
        REQUIRE(cpu.ReadFromMemory(0x0100 + initial_stack_pointer - 1) == (first_return_address & 0xFF));

        // Check second return address
        REQUIRE(cpu.ReadFromMemory(0x0100 + stack_pointer_after_first) == ((second_return_address >> 8) & 0xFF));
        REQUIRE(cpu.ReadFromMemory(0x0100 + stack_pointer_after_first - 1) == (second_return_address & 0xFF));
      }
    }

    WHEN("We execute JSR with different target addresses") {
      auto initial_stack_pointer = cpu.StackPointer();
      original_program_counter = cpu.ProgramCounter();

      // Test JSR to address 0x0000 (edge case)
      auto jsr_zero = CPU::JumpToSubroutine{0x0000};
      cpu.RunInstruction(jsr_zero);

      THEN("JSR to address 0x0000 should work") {
        REQUIRE(cpu.ProgramCounter() == 0x0000);
        REQUIRE(cpu.StackPointer() == initial_stack_pointer - 2);

        auto expected_return_address = original_program_counter + 3 - 1;
        REQUIRE(cpu.ReadFromMemory(0x0100 + initial_stack_pointer) == ((expected_return_address >> 8) & 0xFF));
        REQUIRE(cpu.ReadFromMemory(0x0100 + initial_stack_pointer - 1) == (expected_return_address & 0xFF));
      }

      // Reset for next test
      auto new_initial_sp = cpu.StackPointer() + 2;     // Simulate stack cleanup
      cpu.WriteToMemory(0x0100 + new_initial_sp, 0x00); // Clear stack
      cpu.WriteToMemory(0x0100 + new_initial_sp - 1, 0x00);

      // Test JSR to address 0xFFFF (high address)
      auto jsr_high = CPU::JumpToSubroutine{0xFFFF};
      cpu.RunInstruction(jsr_high);

      THEN("JSR to high address should work") {
        REQUIRE(cpu.ProgramCounter() == 0xFFFF);
        // The stack pointer should be decremented correctly regardless of target address
      }
    }

    WHEN("We execute JSR and verify instruction size and cycles") {
      original_program_counter = cpu.ProgramCounter();

      auto jsr = CPU::JumpToSubroutine{0x2000};

      // Verify instruction properties before execution
      THEN("JSR instruction should have correct size and cycles") {
        REQUIRE(jsr.size == 3);   // JSR is a 3-byte instruction
        REQUIRE(jsr.cycles == 6); // JSR takes 6 cycles
      }

      cpu.RunInstruction(jsr);

      THEN("JSR should jump correctly") { REQUIRE(cpu.ProgramCounter() == 0x2000); }
    }

    WHEN("We execute JSR with stack boundary conditions") {
      // Test JSR when stack is nearly full
      // Set stack pointer to a low value to simulate a nearly full stack
      auto low_stack_pointer = uint8_t(0x02);
      cpu.WriteToMemory(0x0100 + low_stack_pointer, 0x00); // Ensure memory is clear

      // We can't directly set the stack pointer in the public interface,
      // but we can test that JSR handles normal stack operations correctly
      auto initial_stack_pointer = cpu.StackPointer();
      original_program_counter = cpu.ProgramCounter();

      auto jsr = CPU::JumpToSubroutine{0x1000};
      cpu.RunInstruction(jsr);

      THEN("JSR should handle stack operations correctly even with different stack states") {
        REQUIRE(cpu.ProgramCounter() == 0x1000);
        REQUIRE(cpu.StackPointer() == initial_stack_pointer - 2);

        // Verify return address is correctly stored
        auto expected_return_address = original_program_counter + 3 - 1;
        REQUIRE(cpu.ReadFromMemory(0x0100 + initial_stack_pointer) == ((expected_return_address >> 8) & 0xFF));
        REQUIRE(cpu.ReadFromMemory(0x0100 + initial_stack_pointer - 1) == (expected_return_address & 0xFF));
      }
    }

    WHEN("We test JSR return address calculation edge cases") {
      // Test JSR from different starting program counter values
      original_program_counter = cpu.ProgramCounter();

      auto jsr = CPU::JumpToSubroutine{0x8000};
      cpu.RunInstruction(jsr);

      auto expected_return_address = original_program_counter + 3 - 1;

      THEN("Return address should be calculated correctly (PC + 3 - 1)") {
        REQUIRE(cpu.ProgramCounter() == 0x8000);

        // The return address stored should be the address of the last byte of the JSR instruction
        // This is the standard 6502 behavior for JSR
        auto stored_high_byte = cpu.ReadFromMemory(0x0100 + cpu.StackPointer() + 2);
        auto stored_low_byte = cpu.ReadFromMemory(0x0100 + cpu.StackPointer() + 1);
        uint16_t stored_return_address = (stored_high_byte << 8) | stored_low_byte;

        REQUIRE(stored_return_address == expected_return_address);
      }
    }

    WHEN("We execute a RTS instruction") {
      // Test complete JSR/RTS cycle
      auto jsr_instr = CPU::JumpToSubroutine{0x3000};
      auto rts_instr = CPU::ReturnFromSubroutine{};
      auto initial_pc = cpu.ProgramCounter();
      // Execute JSR to set up stack
      cpu.RunInstruction(jsr_instr);
      REQUIRE(cpu.ProgramCounter() == 0x3000);
      auto stack_after_jsr = cpu.StackPointer();
      // Now execute RTS
      cpu.RunInstruction(rts_instr);
      THEN("RTS should restore return address from stack and update program counter") {
        REQUIRE(cpu.StackPointer() == stack_after_jsr + 2); // Stack pointer restored
        REQUIRE(cpu.ProgramCounter() == initial_pc + 3);    // Return to after JSR instruction
      }
    }

    WHEN("We execute multiple JSR/RTS pairs (nested subroutines)") {
      // Test nested subroutine calls
      auto initial_pc = cpu.ProgramCounter();
      auto initial_stack_pointer = cpu.StackPointer();
      auto jsr1 = CPU::JumpToSubroutine{0x3000};
      auto jsr2 = CPU::JumpToSubroutine{0x4000};
      auto rts1 = CPU::ReturnFromSubroutine{};
      auto rts2 = CPU::ReturnFromSubroutine{};
      // First JSR
      cpu.RunInstruction(jsr1);
      THEN("First JSR should work correctly") {
        REQUIRE(cpu.ProgramCounter() == 0x3000);
        REQUIRE(cpu.StackPointer() == initial_stack_pointer - 2);
      }
      // Second JSR (nested call)
      cpu.RunInstruction(jsr2);
      THEN("Second JSR should work correctly") {
        REQUIRE(cpu.ProgramCounter() == 0x4000);
        REQUIRE(cpu.StackPointer() == initial_stack_pointer - 4); // Two return addresses on stack
      }
      // First RTS (return from inner subroutine)
      cpu.RunInstruction(rts1);
      THEN("First RTS should return to outer subroutine") {
        REQUIRE(cpu.ProgramCounter() == 0x3000 + jsr2.size);      // Return to after second JSR in first subroutine
        REQUIRE(cpu.StackPointer() == initial_stack_pointer - 2); // One return address remains
      }
      // Second RTS (return from outer subroutine)
      cpu.RunInstruction(rts2);
      THEN("Second RTS should return to main program") {
        REQUIRE(cpu.ProgramCounter() == initial_pc + jsr1.size); // Return to after first JSR
        REQUIRE(cpu.StackPointer() == initial_stack_pointer);    // Stack fully restored
      }
    }

    WHEN("We execute RTS with different return addresses") {
      // Test RTS with various return address values
      struct TestCase {
        uint16_t target_addr;
        const char *description;
      };
      std::vector<TestCase> test_cases = {
          {0x8100, "Low program address"}, {0x9000, "Mid program address"}, {0xA000, "High program address"}};
      for (const auto &test_case : test_cases) {
        auto jsr_instr = CPU::JumpToSubroutine{test_case.target_addr};
        auto rts_instr = CPU::ReturnFromSubroutine{};
        auto initial_pc = cpu.ProgramCounter();
        // Execute JSR to target address
        cpu.RunInstruction(jsr_instr);
        REQUIRE(cpu.ProgramCounter() == test_case.target_addr);

        // Execute RTS
        cpu.RunInstruction(rts_instr);

        THEN(std::string("RTS should work with ") + test_case.description) {
          REQUIRE(cpu.ProgramCounter() == initial_pc + 3); // Return to after JSR
        }
      }
    }

    WHEN("We execute RTS and verify instruction properties") {
      auto rts_instr = CPU::ReturnFromSubroutine{};

      THEN("RTS instruction should have correct size and cycles") {
        REQUIRE(rts_instr.size == 1);   // RTS is a 1-byte instruction
        REQUIRE(rts_instr.cycles == 6); // RTS takes 6 cycles
      }
    }

    WHEN("We execute a RTI instruction") {
      // RTI should restore both the program counter and status register from the stack
      // Simulate an interrupt by manually setting up the stack as an interrupt would
      auto initial_status = uint8_t(0x24); // Some test status value
      auto return_address = uint16_t(0x1234);

      // Set up the stack as if an interrupt occurred:
      // We need to manually set the stack pointer using TXS instruction
      // Set X register to initial stack pointer value minus 3 (for status, PC_low, PC_high)
      auto target_sp = uint8_t(0xFC); // Start with SP at 0xFC
      cpu.SetRegister(CPU::Register::X, target_sp);
      auto txs_instr = CPU::TransferXToStackPointer{};
      cpu.RunInstruction(txs_instr);

      // Stack layout (from top to bottom): PC_high, PC_low, status
      // Write to the stack positions where RTI will read them
      cpu.WriteToMemory(0x0100 + target_sp + 1, initial_status);        // Status at SP+1
      cpu.WriteToMemory(0x0100 + target_sp + 2, return_address & 0xFF); // Low byte at SP+2
      cpu.WriteToMemory(0x0100 + target_sp + 3, (return_address >> 8) & 0xFF); // High byte at SP+3

      auto stack_before_rti = cpu.StackPointer();

      // Execute RTI
      auto rti_instr = CPU::ReturnFromInterrupt{};
      cpu.RunInstruction(rti_instr);

      THEN("RTI should restore program counter and status register from stack") {
        REQUIRE(cpu.ProgramCounter() == return_address);
        REQUIRE((cpu.StatusFlags().to_ulong() & 0xCF) == (initial_status & 0xCF)); // Ignore bits 4-5 (Break and unused)
        REQUIRE(cpu.StackPointer() == stack_before_rti + 3);                       // Stack pointer restored (3 bytes pulled)
      }
    }

    WHEN("We execute RTI with different status flags") {
      // Test RTI with various status register values
      struct TestCase {
        uint8_t status_value;
        const char *description;
      };
      std::vector<TestCase> test_cases = {{0x00, "All flags clear"},
                                           {0xFF, "All flags set"},
                                           {0x24, "Default/typical value"},
                                           {0x81, "Negative and Carry set"}};

      for (const auto &test_case : test_cases) {
        auto return_address = uint16_t(0x2000);

        // Set up stack pointer using TXS
        auto target_sp = uint8_t(0xFC);
        cpu.SetRegister(CPU::Register::X, target_sp);
        auto txs_instr = CPU::TransferXToStackPointer{};
        cpu.RunInstruction(txs_instr);

        // Set up stack for RTI
        cpu.WriteToMemory(0x0100 + target_sp + 1, test_case.status_value);
        cpu.WriteToMemory(0x0100 + target_sp + 2, return_address & 0xFF);
        cpu.WriteToMemory(0x0100 + target_sp + 3, (return_address >> 8) & 0xFF);

        // Execute RTI
        auto rti_instr = CPU::ReturnFromInterrupt{};
        cpu.RunInstruction(rti_instr);

        THEN(std::string("RTI should restore status correctly with ") + test_case.description) {
          REQUIRE((cpu.StatusFlags().to_ulong() & 0xCF) == (test_case.status_value & 0xCF)); // Check status (ignore bits 4-5)
          REQUIRE(cpu.ProgramCounter() == return_address);
        }
      }
    }

    WHEN("We execute RTI with different return addresses") {
      // Test RTI with various return address values
      struct TestCase {
        uint16_t return_addr;
        const char *description;
      };
      std::vector<TestCase> test_cases = {
          {0x8000, "Start of program ROM"}, {0xC000, "Mid program ROM"}, {0xFFFC, "Near interrupt vectors"}};

      for (const auto &test_case : test_cases) {
        auto status_value = uint8_t(0x24);

        // Set up stack pointer using TXS
        auto target_sp = uint8_t(0xFC);
        cpu.SetRegister(CPU::Register::X, target_sp);
        auto txs_instr = CPU::TransferXToStackPointer{};
        cpu.RunInstruction(txs_instr);

        // Set up stack for RTI
        cpu.WriteToMemory(0x0100 + target_sp + 1, status_value);
        cpu.WriteToMemory(0x0100 + target_sp + 2, test_case.return_addr & 0xFF);
        cpu.WriteToMemory(0x0100 + target_sp + 3, (test_case.return_addr >> 8) & 0xFF);

        // Execute RTI
        auto rti_instr = CPU::ReturnFromInterrupt{};
        cpu.RunInstruction(rti_instr);

        THEN(std::string("RTI should work with ") + test_case.description) {
          REQUIRE(cpu.ProgramCounter() == test_case.return_addr);
        }
      }
    }

    WHEN("We execute RTI and verify instruction properties") {
      auto rti_instr = CPU::ReturnFromInterrupt{};

      THEN("RTI instruction should have correct size and cycles") {
        REQUIRE(rti_instr.size == 1);   // RTI is a 1-byte instruction
        REQUIRE(rti_instr.cycles == 6); // RTI takes 6 cycles
      }
    }
  }
}
