//
// Created by Valerio Formato on 25-Jul-25.
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
  using CPU::SetStatusFlagValue;
  using CPU::WriteToMemory;
};

SCENARIO("6502 instruction execution tests (math ops)") {
  GIVEN("A freshly initialized cpu") {
    CPUMock cpu;
    auto original_program_counter = cpu.ProgramCounter();

    WHEN("We execute a INX instruction") {
      auto load_instr = CPU::LoadRegister<CPU::Register::X, AddressingMode::Immediate>{0x42};
      auto inx_instr = CPU::IncrementRegister<CPU::Register::X>{};

      // First, load the X register with a value
      cpu.RunInstruction(load_instr);

      // Now execute the INX instruction
      cpu.RunInstruction(inx_instr);

      THEN("The X register should be incremented by 1") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x43);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      load_instr.value = 0xFF; // Change operand to 0xFF
      cpu.RunInstruction(load_instr);
      cpu.RunInstruction(inx_instr);

      THEN("The X register should wrap around to 0 and the zero flag should be set") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute a INY instruction") {
      auto load_instr = CPU::LoadRegister<CPU::Register::Y, AddressingMode::Immediate>{0x42};
      auto iny_instr = CPU::IncrementRegister<CPU::Register::Y>{};

      // First, load the Y register with a value
      cpu.RunInstruction(load_instr);

      // Now execute the INY instruction
      cpu.RunInstruction(iny_instr);

      THEN("The Y register should be incremented by 1") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x43);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      load_instr.value = 0xFF; // Change operand to 0xFF
      cpu.RunInstruction(load_instr);
      cpu.RunInstruction(iny_instr);

      THEN("The Y register should wrap around to 0 and the zero flag should be set") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute an INC zero-page instruction") {
      // Setup memory for zero-page test
      cpu.WriteToMemory(0x42, 0x41);

      auto instr = CPU::Increment<AddressingMode::ZeroPage>{0x42};
      cpu.RunInstruction(instr);

      THEN("The memory should contain the incremented value 0x42, with all flags clear") {
        REQUIRE(cpu.ReadFromMemory(0x42) == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x43, 0xFF);
      instr.address = 0x43;
      cpu.RunInstruction(instr);

      THEN("The memory should contain 0x00, with zero flag set") {
        REQUIRE(cpu.ReadFromMemory(0x43) == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x44, 0x7F);
      instr.address = 0x44;
      cpu.RunInstruction(instr);

      THEN("The memory should contain 0x80, with negative flag set") {
        REQUIRE(cpu.ReadFromMemory(0x44) == 0x80);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute an INC zero-page,X instruction") {
      // Setup memory and X register for zero-page,X test
      cpu.SetRegister(CPU::Register::X, 0x05);
      cpu.WriteToMemory(0x47, 0x80); // 0x42 + 0x05 = 0x47

      auto instr = CPU::Increment<AddressingMode::ZeroPageX>{0x42};
      cpu.RunInstruction(instr);

      THEN("The memory should contain the incremented value 0x81, with negative flag set") {
        REQUIRE(cpu.ReadFromMemory(0x47) == 0x81);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }

      original_program_counter = cpu.ProgramCounter();

      // Test zero page wrap around
      cpu.SetRegister(CPU::Register::X, 0xFF);
      cpu.WriteToMemory(0x01, 0x7E); // 0x02 + 0xFF = 0x101, wraps to 0x01
      instr.address = 0x02;
      cpu.RunInstruction(instr);

      THEN("The memory should wrap around in zero page and increment correctly") {
        REQUIRE(cpu.ReadFromMemory(0x01) == 0x7F);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute an INC absolute instruction") {
      // Setup memory for absolute test
      cpu.WriteToMemory(0x0300, 0x10);

      auto instr = CPU::Increment<AddressingMode::Absolute>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The memory should contain the incremented value 0x11, with all flags clear") {
        REQUIRE(cpu.ReadFromMemory(0x0300) == 0x11);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x0301, 0xFF);
      instr.address = 0x0301;
      cpu.RunInstruction(instr);

      THEN("The memory should wrap to 0x00, with zero flag set") {
        REQUIRE(cpu.ReadFromMemory(0x0301) == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x0302, 0x7F);
      instr.address = 0x0302;
      cpu.RunInstruction(instr);

      THEN("The memory should contain 0x80, with negative flag set") {
        REQUIRE(cpu.ReadFromMemory(0x0302) == 0x80);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute an INC absolute,X instruction") {
      // Setup memory and X register for absolute,X test
      cpu.SetRegister(CPU::Register::X, 0x10);
      cpu.WriteToMemory(0x0310, 0x0F); // 0x0300 + 0x10 = 0x0310

      auto instr = CPU::Increment<AddressingMode::AbsoluteX>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The memory should contain the incremented value 0x10, with all flags clear") {
        REQUIRE(cpu.ReadFromMemory(0x0310) == 0x10);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::X, 0x20);
      cpu.WriteToMemory(0x0320, 0xFF); // 0x0300 + 0x20 = 0x0320
      cpu.RunInstruction(instr);

      THEN("The memory should wrap to 0x00, with zero flag set") {
        REQUIRE(cpu.ReadFromMemory(0x0320) == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::X, 0x30);
      cpu.WriteToMemory(0x0330, 0x7F); // 0x0300 + 0x30 = 0x0330
      cpu.RunInstruction(instr);

      THEN("The memory should contain 0x80, with negative flag set") {
        REQUIRE(cpu.ReadFromMemory(0x0330) == 0x80);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a DEX instruction") {
      auto load_instr = CPU::LoadRegister<CPU::Register::X, AddressingMode::Immediate>{0x42};
      auto inx_instr = CPU::DecrementRegister<CPU::Register::X>{};

      // First, load the X register with a value
      cpu.RunInstruction(load_instr);

      // Now execute the DEX instruction
      cpu.RunInstruction(inx_instr);

      THEN("The X register should be incremented by 1") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x41);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      load_instr.value = 0x00; // Change operand to 0x00
      cpu.RunInstruction(load_instr);
      cpu.RunInstruction(inx_instr);

      THEN("The X register should wrap around to 0xFF and the negative flag should be set") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0xFF);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a DEY instruction") {
      auto load_instr = CPU::LoadRegister<CPU::Register::Y, AddressingMode::Immediate>{0x42};
      auto iny_instr = CPU::DecrementRegister<CPU::Register::Y>{};

      // First, load the Y register with a value
      cpu.RunInstruction(load_instr);

      // Now execute the DEY instruction
      cpu.RunInstruction(iny_instr);

      THEN("The Y register should be incremented by 1") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x41);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      load_instr.value = 0x00; // Change operand to 0x00
      cpu.RunInstruction(load_instr);
      cpu.RunInstruction(iny_instr);

      THEN("The Y register should wrap around to 0xFF and the negative flag should be set") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0xFF);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a DEC zero-page instruction") {
      // Setup memory for zero-page test
      cpu.WriteToMemory(0x42, 0x02);

      auto instr = CPU::Decrement<AddressingMode::ZeroPage>{0x42};
      cpu.RunInstruction(instr);

      THEN("The memory should contain the decremented value 0x01, with all flags clear") {
        REQUIRE(cpu.ReadFromMemory(0x42) == 0x01);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x43, 0x01);
      instr.address = 0x43;
      cpu.RunInstruction(instr);

      THEN("The memory should contain 0x00, with zero flag set") {
        REQUIRE(cpu.ReadFromMemory(0x43) == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x44, 0x00);
      instr.address = 0x44;
      cpu.RunInstruction(instr);

      THEN("The memory should contain 0xFF, with negative flag set") {
        REQUIRE(cpu.ReadFromMemory(0x44) == 0xFF);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a DEC zero-page,X instruction") {
      // Setup memory and X register for zero-page,X test
      cpu.SetRegister(CPU::Register::X, 0x05);
      cpu.WriteToMemory(0x47, 0x80); // 0x42 + 0x05 = 0x47

      auto instr = CPU::Decrement<AddressingMode::ZeroPageX>{0x42};
      cpu.RunInstruction(instr);

      THEN("The memory should contain the decremented value 0x7F, with all flags clear") {
        REQUIRE(cpu.ReadFromMemory(0x47) == 0x7F);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute a DEC absolute instruction") {
      // Setup memory for absolute test
      cpu.WriteToMemory(0x0300, 0x81);

      auto instr = CPU::Decrement<AddressingMode::Absolute>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The memory should contain the decremented value 0x80, with negative flag set") {
        REQUIRE(cpu.ReadFromMemory(0x0300) == 0x80);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a DEC absolute,X instruction") {
      // Setup memory and X register for absolute,X test
      cpu.SetRegister(CPU::Register::X, 0x10);
      cpu.WriteToMemory(0x0310, 0x10); // 0x0300 + 0x10 = 0x0310

      auto instr = CPU::Decrement<AddressingMode::AbsoluteX>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The memory should contain the decremented value 0x0F, with all flags clear") {
        REQUIRE(cpu.ReadFromMemory(0x0310) == 0x0F);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute a ADC immediate instruction") {
      cpu.SetRegister(CPU::Register::A, 0x02);

      auto instr = CPU::AddWithCarry<AddressingMode::Immediate>{0x40};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the immediate value 0x42, with zero flag clear and negative flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0x02);
      instr.value = 0xFE;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x00, with zero,carry flags set and negative flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0x02);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      instr.value = 0x80;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x83, with negative flag set and zero,carry flags clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x83);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 80);
      instr.value = 80;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 160, with negative flag set and zero,carry flags clear but overflow flag "
           "set!") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 160);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == true);
      }
    }

    WHEN("We execute a ADC zero-page instruction") {
      cpu.SetRegister(CPU::Register::A, 0x02);
      cpu.WriteToMemory(0x50, 0x40); // Write test value to zero-page address

      auto instr = CPU::AddWithCarry<AddressingMode::ZeroPage>{0x50};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the zero-page value 0x42, with zero flag clear and negative flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0x02);
      cpu.WriteToMemory(0x51, 0xFE); // Write test value to zero-page address
      instr.value = 0x51;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x00, with zero,carry flags set and negative flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0x02);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.WriteToMemory(0x52, 0x80); // Write test value to zero-page address
      instr.value = 0x52;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x83, with negative flag set and zero,carry flags clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x83);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 80);
      cpu.WriteToMemory(0x53, 80); // Write test value to zero-page address
      instr.value = 0x53;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 160, with negative flag set and zero,carry flags clear but overflow flag "
           "set!") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 160);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == true);
      }
    }

    WHEN("We execute a ADC zero-page,X instruction") {
      cpu.SetRegister(CPU::Register::A, 0x02);
      cpu.SetRegister(CPU::Register::X, 0x05);
      cpu.WriteToMemory(0x55, 0x40); // Write test value to zero-page address 0x50 + 0x05

      auto instr = CPU::AddWithCarry<AddressingMode::ZeroPageX>{0x50};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the zero-page,X value 0x42") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }
    }

    WHEN("We execute a ADC absolute instruction") {
      cpu.SetRegister(CPU::Register::A, 0x02);
      cpu.WriteToMemory(0x0300, 0x40); // Write test value to valid RAM address

      auto instr = CPU::AddWithCarry<AddressingMode::Absolute>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the absolute value 0x42") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }
    }

    WHEN("We execute a ADC absolute,X instruction") {
      cpu.SetRegister(CPU::Register::A, 0x02);
      cpu.SetRegister(CPU::Register::X, 0x05);
      cpu.WriteToMemory(0x0305, 0x40); // Write test value to valid RAM address + X

      auto instr = CPU::AddWithCarry<AddressingMode::AbsoluteX>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the absolute,X value 0x42") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }
    }

    WHEN("We execute a ADC absolute,Y instruction") {
      cpu.SetRegister(CPU::Register::A, 0x02);
      cpu.SetRegister(CPU::Register::Y, 0x05);
      cpu.WriteToMemory(0x0305, 0x40); // Write test value to valid RAM address + Y

      auto instr = CPU::AddWithCarry<AddressingMode::AbsoluteY>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the absolute,Y value 0x42") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }
    }

    WHEN("We execute a ADC indirect,X instruction") {
      cpu.SetRegister(CPU::Register::A, 0x02);
      cpu.SetRegister(CPU::Register::X, 0x05);

      // Set up indirect addressing: pointer at 0x25 (0x20 + 0x05) points to 0x0300
      cpu.WriteToMemory(0x25, 0x00);   // Low byte of target address
      cpu.WriteToMemory(0x26, 0x03);   // High byte of target address
      cpu.WriteToMemory(0x0300, 0x40); // Value at target address

      auto instr = CPU::AddWithCarry<AddressingMode::IndirectX>{0x20};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the indirect,X value 0x42") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }
    }

    WHEN("We execute a ADC indirect,Y instruction") {
      cpu.SetRegister(CPU::Register::A, 0x02);
      cpu.SetRegister(CPU::Register::Y, 0x05);

      // Set up indirect addressing: pointer at 0x20 points to 0x0300, then add Y
      cpu.WriteToMemory(0x20, 0x00);   // Low byte of base address
      cpu.WriteToMemory(0x21, 0x03);   // High byte of base address
      cpu.WriteToMemory(0x0305, 0x40); // Value at target address (0x0300 + 0x05)

      auto instr = CPU::AddWithCarry<AddressingMode::IndirectY>{0x20};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the indirect,Y value 0x42") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }
    }
  }
}

SCENARIO("6502 instruction execution tests (logical ops)") {
  GIVEN("A freshly initialized cpu") {
    CPUMock cpu;
    auto original_program_counter = cpu.ProgramCounter();

    WHEN("We execute a AND immediate instruction") {
      cpu.SetRegister(CPU::Register::A, 0xFF);

      auto instr = CPU::LogicalAND<AddressingMode::Immediate>{0x0F};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the masked value 0x0F, with zero flag clear and negative flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x0F);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0xAA);
      instr.value = 0x55;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x00, with zero flag set and negative flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0xFF);
      instr.value = 0x80;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x80, with negative flag set and zero flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x80);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0x7F);
      instr.value = 0x7F;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x7F, with both zero and negative flags clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x7F);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute a AND zero-page instruction") {
      // Setup memory for zero-page test
      cpu.WriteToMemory(0x42, 0x0F);
      cpu.SetRegister(CPU::Register::A, 0xFF);

      auto instr = CPU::LogicalAND<AddressingMode::ZeroPage>{0x42};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the masked value from memory") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x0F);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute a AND absolute instruction") {
      // Setup memory for absolute test
      cpu.WriteToMemory(0x0300, 0x80);
      cpu.SetRegister(CPU::Register::A, 0xFF);

      auto instr = CPU::LogicalAND<AddressingMode::Absolute>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the masked value from memory with negative flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x80);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a ASL accumulator instruction") {
      cpu.SetRegister(CPU::Register::A, 0x40);

      auto instr = CPU::ShiftLeft<AddressingMode::Accumulator>{0x00};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the shifted value 0x80, with negative flag set and zero,carry flags clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x80);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0x80);
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x00, with zero and carry flags set, negative flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0x3F);
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x7E, with all flags clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x7E);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0xC0);
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x80, with negative and carry flags set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x80);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
      }
    }

    WHEN("We execute a ASL zero-page instruction") {
      // Setup memory for zero-page test
      cpu.WriteToMemory(0x42, 0x40);

      auto instr = CPU::ShiftLeft<AddressingMode::ZeroPage>{0x42};
      cpu.RunInstruction(instr);

      THEN("The memory should contain the shifted value 0x80, with negative flag set") {
        REQUIRE(cpu.ReadFromMemory(0x42) == 0x80);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x43, 0x80);
      instr.address = 0x43;
      cpu.RunInstruction(instr);

      THEN("The memory should contain 0x00, with zero and carry flags set") {
        REQUIRE(cpu.ReadFromMemory(0x43) == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
      }
    }

    WHEN("We execute a ASL absolute instruction") {
      // Setup memory for absolute test
      cpu.WriteToMemory(0x0300, 0x3F);

      auto instr = CPU::ShiftLeft<AddressingMode::Absolute>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The memory should contain the shifted value 0x7E, with all flags clear") {
        REQUIRE(cpu.ReadFromMemory(0x0300) == 0x7E);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
      }
    }

    WHEN("We execute a LSR accumulator instruction") {
      cpu.SetRegister(CPU::Register::A, 0x40);

      auto instr = CPU::ShiftRight<AddressingMode::Accumulator>{0x00};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the shifted value 0x80, with negative flag set and zero,carry flags clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x20);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0x01);
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x00, with zero and carry flags set, negative flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0x7E);
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x3F, with all flags clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x3F);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0x3);
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x01, with carry flags set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x01);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
      }
    }

    WHEN("We execute a LSR zero-page instruction") {
      // Setup memory for zero-page test
      cpu.WriteToMemory(0x42, 0x40);

      auto instr = CPU::ShiftRight<AddressingMode::ZeroPage>{0x42};
      cpu.RunInstruction(instr);

      THEN("The memory should contain the shifted value 0x20") {
        REQUIRE(cpu.ReadFromMemory(0x42) == 0x20);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x43, 0x01);
      instr.address = 0x43;
      cpu.RunInstruction(instr);

      THEN("The memory should contain 0x00, with zero and carry flags set") {
        REQUIRE(cpu.ReadFromMemory(0x43) == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
      }
    }

    WHEN("We execute a LSR absolute instruction") {
      // Setup memory for absolute test
      cpu.WriteToMemory(0x0300, 0x7E);

      auto instr = CPU::ShiftRight<AddressingMode::Absolute>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The memory should contain the shifted value 0x3F, with all flags clear") {
        REQUIRE(cpu.ReadFromMemory(0x0300) == 0x3F);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
      }
    }

    WHEN("We execute a EOR immediate instruction") {
      cpu.SetRegister(CPU::Register::A, 0xFF);

      auto instr = CPU::ExclusiveOR<AddressingMode::Immediate>{0x0F};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the XOR result 0xF0, with negative flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xF0);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0xAA);
      instr.value = 0xAA;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x00, with zero flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0x55);
      instr.value = 0xAA;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0xFF, with negative flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFF);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0x7F);
      instr.value = 0x00;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x7F, with both zero and negative flags clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x7F);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute a EOR zero-page instruction") {
      // Setup memory for zero-page test
      cpu.WriteToMemory(0x42, 0x0F);
      cpu.SetRegister(CPU::Register::A, 0xFF);

      auto instr = CPU::ExclusiveOR<AddressingMode::ZeroPage>{0x42};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the XOR result from memory") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xF0);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x43, 0x55);
      cpu.SetRegister(CPU::Register::A, 0x55);
      instr.value = 0x43;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x00, with zero flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute a EOR zero-page,X instruction") {
      // Setup memory and X register for zero-page,X test
      cpu.SetRegister(CPU::Register::X, 0x05);
      cpu.WriteToMemory(0x47, 0x3C); // 0x42 + 0x05 = 0x47
      cpu.SetRegister(CPU::Register::A, 0xC3);

      auto instr = CPU::ExclusiveOR<AddressingMode::ZeroPageX>{0x42};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the XOR result from zero-page,X") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFF);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a EOR absolute instruction") {
      // Setup memory for absolute test
      cpu.WriteToMemory(0x0300, 0x81);
      cpu.SetRegister(CPU::Register::A, 0x01);

      auto instr = CPU::ExclusiveOR<AddressingMode::Absolute>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the XOR result from memory with negative flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x80);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a EOR absolute,X instruction") {
      // Setup memory and X register for absolute,X test
      cpu.SetRegister(CPU::Register::X, 0x05);
      cpu.WriteToMemory(0x0305, 0x40); // 0x0300 + 0x05 = 0x0305
      cpu.SetRegister(CPU::Register::A, 0x02);

      auto instr = CPU::ExclusiveOR<AddressingMode::AbsoluteX>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the XOR result from absolute,X") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute a EOR absolute,Y instruction") {
      // Setup memory and Y register for absolute,Y test
      cpu.SetRegister(CPU::Register::Y, 0x05);
      cpu.WriteToMemory(0x0305, 0x7E); // 0x0300 + 0x05 = 0x0305
      cpu.SetRegister(CPU::Register::A, 0x01);

      auto instr = CPU::ExclusiveOR<AddressingMode::AbsoluteY>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the XOR result from absolute,Y") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x7F);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute a EOR indirect,X instruction") {
      cpu.SetRegister(CPU::Register::A, 0x0F);
      cpu.SetRegister(CPU::Register::X, 0x05);

      // Set up indirect addressing: pointer at 0x25 (0x20 + 0x05) points to 0x0300
      cpu.WriteToMemory(0x25, 0x00);   // Low byte of target address
      cpu.WriteToMemory(0x26, 0x03);   // High byte of target address
      cpu.WriteToMemory(0x0300, 0xF0); // Value at target address

      auto instr = CPU::ExclusiveOR<AddressingMode::IndirectX>{0x20};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the XOR result from indirect,X") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFF);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a EOR indirect,Y instruction") {
      cpu.SetRegister(CPU::Register::A, 0x33);
      cpu.SetRegister(CPU::Register::Y, 0x05);

      // Set up indirect addressing: pointer at 0x20 points to 0x0300, then add Y
      cpu.WriteToMemory(0x20, 0x00);   // Low byte of base address
      cpu.WriteToMemory(0x21, 0x03);   // High byte of base address
      cpu.WriteToMemory(0x0305, 0xCC); // Value at target address (0x0300 + 0x05)

      auto instr = CPU::ExclusiveOR<AddressingMode::IndirectY>{0x20};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the XOR result from indirect,Y") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFF);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }
  }
}
