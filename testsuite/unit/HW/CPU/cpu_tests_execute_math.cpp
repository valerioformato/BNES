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
  CPUMock(BNES::HW::Bus &bus) : CPU(bus) {}

  using CPU::ReadFromMemory;
  using CPU::SetRegister;
  using CPU::SetStatusFlagValue;
  using CPU::WriteToMemory;
};

SCENARIO("6502 instruction execution tests (math ops)") {
  GIVEN("A freshly initialized cpu") {
    Bus bus;
    CPUMock cpu{bus};
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

    WHEN("We execute ADC indexed indirect with zero-page wrapping at $FF") {
      cpu.SetRegister(CPU::Register::A, 0x10);
      cpu.SetRegister(CPU::Register::X, 0x00);

      // Set up zero page wrapping: $FF/$00 -> $0400
      cpu.WriteToMemory(0x00FF, 0x00); // Low byte
      cpu.WriteToMemory(0x0000, 0x04); // High byte (wraps)
      cpu.WriteToMemory(0x0100, 0x00); // Ensure buggy code fails
      cpu.WriteToMemory(0x0400, 0x25); // Value to add

      auto instr = CPU::AddWithCarry<AddressingMode::IndirectX>{0xFF};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x35 (0x10 + 0x25)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x35);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
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

    WHEN("We execute ADC indirect indexed with zero-page wrapping at $FF") {
      cpu.SetRegister(CPU::Register::A, 0x15);
      cpu.SetRegister(CPU::Register::Y, 0x03);

      // Set up zero page wrapping: $FF/$00 -> base $0400, then add Y
      cpu.WriteToMemory(0x00FF, 0x00); // Low byte of base address
      cpu.WriteToMemory(0x0000, 0x04); // High byte (wraps to $00)
      cpu.WriteToMemory(0x0100, 0x00); // Ensure buggy code fails
      cpu.WriteToMemory(0x0403, 0x2A); // Value at $0400 + Y ($03)

      auto instr = CPU::AddWithCarry<AddressingMode::IndirectY>{0xFF};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x3F (0x15 + 0x2A)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x3F);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
    }

    WHEN("We execute a SBC immediate instruction") {
      cpu.SetRegister(CPU::Register::A, 0x50);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true); // Carry flag must be set for no borrow

      auto instr = CPU::SubtractWithCarry<AddressingMode::Immediate>{0x10};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x40, with flags clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x40);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0x50);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      instr.value = 0x50;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x00, with zero and carry flags set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0x50);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, false); // Borrow is needed
      instr.value = 0x10;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x3F, with carry set (result is 0x50 - 0x10 - 1)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x3F);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0x50);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      instr.value = 0xB0;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0xA0, with negative flag set and carry clear (borrow occurred)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xA0);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == true); // Overflow: positive - negative = negative
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0x50); // 80 in decimal (positive)
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      instr.value = 0xF0; // -16 in two's complement
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x60, no overflow") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x60);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false); // No overflow: same as immediate mode case
      }
    }

    WHEN("We execute a SBC zero-page instruction") {
      cpu.SetRegister(CPU::Register::A, 0x50);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.WriteToMemory(0x50, 0x10); // Write test value to zero-page address

      auto instr = CPU::SubtractWithCarry<AddressingMode::ZeroPage>{0x50};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x40, with flags clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x40);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0x50);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.WriteToMemory(0x51, 0x50);
      instr.value = 0x51;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x00, with zero and carry flags set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0x50); // 80 in decimal
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.WriteToMemory(0x52, 0xF0); // -16 in two's complement
      instr.value = 0x52;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x60, no overflow") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x60);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false); // No overflow: same as immediate mode case
      }
    }

    WHEN("We execute a SBC zero-page,X instruction") {
      cpu.SetRegister(CPU::Register::A, 0x50);
      cpu.SetRegister(CPU::Register::X, 0x05);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.WriteToMemory(0x55, 0x10); // Write test value to zero-page address 0x50 + 0x05

      auto instr = CPU::SubtractWithCarry<AddressingMode::ZeroPageX>{0x50};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the zero-page,X value 0x40") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x40);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }
    }

    WHEN("We execute a SBC absolute instruction") {
      cpu.SetRegister(CPU::Register::A, 0x50);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.WriteToMemory(0x0300, 0x10); // Write test value to valid RAM address

      auto instr = CPU::SubtractWithCarry<AddressingMode::Absolute>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the absolute value 0x40") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x40);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }
    }

    WHEN("We execute a SBC absolute,X instruction") {
      cpu.SetRegister(CPU::Register::A, 0x50);
      cpu.SetRegister(CPU::Register::X, 0x05);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.WriteToMemory(0x0305, 0x10); // Write test value to valid RAM address + X

      auto instr = CPU::SubtractWithCarry<AddressingMode::AbsoluteX>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the absolute,X value 0x40") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x40);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }
    }

    WHEN("We execute a SBC absolute,Y instruction") {
      cpu.SetRegister(CPU::Register::A, 0x50);
      cpu.SetRegister(CPU::Register::Y, 0x05);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.WriteToMemory(0x0305, 0x10); // Write test value to valid RAM address + Y

      auto instr = CPU::SubtractWithCarry<AddressingMode::AbsoluteY>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the absolute,Y value 0x40") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x40);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }
    }

    WHEN("We execute a SBC indirect,X instruction") {
      cpu.SetRegister(CPU::Register::A, 0x50);
      cpu.SetRegister(CPU::Register::X, 0x05);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);

      // Set up indirect addressing: pointer at 0x25 (0x20 + 0x05) points to 0x0300
      cpu.WriteToMemory(0x25, 0x00);   // Low byte of target address
      cpu.WriteToMemory(0x26, 0x03);   // High byte of target address
      cpu.WriteToMemory(0x0300, 0x10); // Value at target address

      auto instr = CPU::SubtractWithCarry<AddressingMode::IndirectX>{0x20};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the indirect,X value 0x40") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x40);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }
    }

    WHEN("We execute SBC indexed indirect with zero-page wrapping at $FF") {
      cpu.SetRegister(CPU::Register::A, 0x50);
      cpu.SetRegister(CPU::Register::X, 0x00);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);

      // Set up zero page wrapping: $FF/$00 -> $0400
      cpu.WriteToMemory(0x00FF, 0x00); // Low byte
      cpu.WriteToMemory(0x0000, 0x04); // High byte (wraps)
      cpu.WriteToMemory(0x0100, 0x00); // Ensure buggy code fails
      cpu.WriteToMemory(0x0400, 0x20); // Value to subtract

      auto instr = CPU::SubtractWithCarry<AddressingMode::IndirectX>{0xFF};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x30 (0x50 - 0x20)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x30);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
    }

    WHEN("We execute a SBC indirect,Y instruction") {
      cpu.SetRegister(CPU::Register::A, 0x50);
      cpu.SetRegister(CPU::Register::Y, 0x05);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);

      // Set up indirect addressing: pointer at 0x20 points to 0x0300, then add Y
      cpu.WriteToMemory(0x20, 0x00);   // Low byte of base address
      cpu.WriteToMemory(0x21, 0x03);   // High byte of base address
      cpu.WriteToMemory(0x0305, 0x10); // Value at target address (0x0300 + 0x05)

      auto instr = CPU::SubtractWithCarry<AddressingMode::IndirectY>{0x20};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the indirect,Y value 0x40") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x40);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Overflow) == false);
      }
    }

    WHEN("We execute SBC indirect indexed with zero-page wrapping at $FF") {
      cpu.SetRegister(CPU::Register::A, 0x60);
      cpu.SetRegister(CPU::Register::Y, 0x04);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);

      // Set up zero page wrapping: $FF/$00 -> base $0400, then add Y
      cpu.WriteToMemory(0x00FF, 0x00); // Low byte of base address
      cpu.WriteToMemory(0x0000, 0x04); // High byte (wraps to $00)
      cpu.WriteToMemory(0x0100, 0x00); // Ensure buggy code fails
      cpu.WriteToMemory(0x0404, 0x25); // Value at $0400 + Y ($04)

      auto instr = CPU::SubtractWithCarry<AddressingMode::IndirectY>{0xFF};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x3B (0x60 - 0x25)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x3B);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
    }
  }
}

SCENARIO("6502 instruction execution tests (logical ops)") {
  GIVEN("A freshly initialized cpu") {
    Bus bus;
    CPUMock cpu{bus};
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

    WHEN("We execute AND indexed indirect with zero-page wrapping at $FF") {
      cpu.SetRegister(CPU::Register::A, 0xFF);
      cpu.SetRegister(CPU::Register::X, 0x00);

      // Set up zero page wrapping: $FF/$00 -> $0400
      cpu.WriteToMemory(0x00FF, 0x00); // Low byte
      cpu.WriteToMemory(0x0000, 0x04); // High byte (wraps)
      cpu.WriteToMemory(0x0100, 0x00); // Ensure buggy code fails
      cpu.WriteToMemory(0x0400, 0x0F); // Value to AND

      auto instr = CPU::LogicalAND<AddressingMode::IndirectX>{0xFF};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x0F (0xFF AND 0x0F)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x0F);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
    }

    WHEN("We execute AND indirect indexed with zero-page wrapping at $FF") {
      cpu.SetRegister(CPU::Register::A, 0xAA);
      cpu.SetRegister(CPU::Register::Y, 0x02);

      // Set up zero page wrapping: $FF/$00 -> base $0400, then add Y
      cpu.WriteToMemory(0x00FF, 0x00); // Low byte of base address
      cpu.WriteToMemory(0x0000, 0x04); // High byte (wraps to $00)
      cpu.WriteToMemory(0x0100, 0x00); // Ensure buggy code fails
      cpu.WriteToMemory(0x0402, 0x55); // Value at $0400 + Y ($02)

      auto instr = CPU::LogicalAND<AddressingMode::IndirectY>{0xFF};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x00 (0xAA AND 0x55)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
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

    WHEN("We execute a ROR accumulator instruction") {
      // Test 1: ROR with carry clear, even value
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, false);
      cpu.SetRegister(CPU::Register::A, 0x40);

      auto instr = CPU::RotateRight<AddressingMode::Accumulator>{0x00};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x20 (carry was 0, bit 0 becomes carry)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x20);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      // Test 2: ROR with carry clear, odd value (bit 0 set)
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, false);
      cpu.SetRegister(CPU::Register::A, 0x41);
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x20 with carry set (old bit 0)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x20);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
      }
      original_program_counter = cpu.ProgramCounter();

      // Test 3: ROR with carry set
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.SetRegister(CPU::Register::A, 0x02);
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x81 (carry 1 goes to bit 7, bit 0 clear becomes new carry)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x81);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true); // Bit 7 is set
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      // Test 4: ROR resulting in zero
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, false);
      cpu.SetRegister(CPU::Register::A, 0x01);
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x00 with zero and carry flags set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
      }
    }

    WHEN("We execute a ROR zero-page instruction") {
      // Test with carry clear
      cpu.WriteToMemory(0x42, 0x80);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, false);

      auto instr = CPU::RotateRight<AddressingMode::ZeroPage>{0x42};
      cpu.RunInstruction(instr);

      THEN("The memory should contain 0x40 with carry clear") {
        REQUIRE(cpu.ReadFromMemory(0x42) == 0x40);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      // Test with carry set
      cpu.WriteToMemory(0x43, 0x00);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      instr.address = 0x43;
      cpu.RunInstruction(instr);

      THEN("The memory should contain 0x80 (carry goes to bit 7) with negative flag set") {
        REQUIRE(cpu.ReadFromMemory(0x43) == 0x80);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
      }
    }

    WHEN("We execute a ROR absolute instruction") {
      // Test rotation with both carry and bit 0 set
      cpu.WriteToMemory(0x0300, 0xFF);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);

      auto instr = CPU::RotateRight<AddressingMode::Absolute>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The memory should contain 0xFF (all bits set, rotates into itself) with carry set") {
        REQUIRE(cpu.ReadFromMemory(0x0300) == 0xFF);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
      }
    }

    WHEN("We execute a ROL accumulator instruction") {
      // Test 1: ROL with carry clear, even value
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, false);
      cpu.SetRegister(CPU::Register::A, 0x40);

      auto instr = CPU::RotateLeft<AddressingMode::Accumulator>{0x00};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x80 (carry was 0, bit 7 becomes carry)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x80);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      // Test 2: ROL with carry clear, bit 7 set
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, false);
      cpu.SetRegister(CPU::Register::A, 0x82);
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x04 with carry set (old bit 7)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x04);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
      }
      original_program_counter = cpu.ProgramCounter();

      // Test 3: ROL with carry set
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.SetRegister(CPU::Register::A, 0x02);
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x05 (carry 1 goes to bit 0, bit 7 clear becomes new carry)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x05);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      // Test 4: ROL resulting in zero
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, false);
      cpu.SetRegister(CPU::Register::A, 0x80);
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x00 with zero and carry flags set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 1);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
      }
    }

    WHEN("We execute a ROL zero-page instruction") {
      // Test with carry clear
      cpu.WriteToMemory(0x42, 0x01);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, false);

      auto instr = CPU::RotateLeft<AddressingMode::ZeroPage>{0x42};
      cpu.RunInstruction(instr);

      THEN("The memory should contain 0x02 with carry clear") {
        REQUIRE(cpu.ReadFromMemory(0x42) == 0x02);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      // Test with carry set
      cpu.WriteToMemory(0x43, 0x00);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      instr.address = 0x43;
      cpu.RunInstruction(instr);

      THEN("The memory should contain 0x01 (carry goes to bit 0) with zero flag clear") {
        REQUIRE(cpu.ReadFromMemory(0x43) == 0x01);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);
      }
    }

    WHEN("We execute a ROL absolute instruction") {
      // Test rotation with both carry and bit 7 set
      cpu.WriteToMemory(0x0300, 0xFF);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);

      auto instr = CPU::RotateLeft<AddressingMode::Absolute>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The memory should contain 0xFF (all bits set, rotates into itself) with carry set") {
        REQUIRE(cpu.ReadFromMemory(0x0300) == 0xFF);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
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

    WHEN("We execute EOR indexed indirect with zero-page wrapping at $FF") {
      cpu.SetRegister(CPU::Register::A, 0x0F);
      cpu.SetRegister(CPU::Register::X, 0x00);

      // Set up zero page wrapping: $FF/$00 -> $0400
      cpu.WriteToMemory(0x00FF, 0x00); // Low byte
      cpu.WriteToMemory(0x0000, 0x04); // High byte (wraps)
      cpu.WriteToMemory(0x0100, 0x00); // Ensure buggy code fails
      cpu.WriteToMemory(0x0400, 0xF0); // Value to XOR

      auto instr = CPU::ExclusiveOR<AddressingMode::IndirectX>{0xFF};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0xFF (0x0F XOR 0xF0)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFF);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
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

    WHEN("We execute EOR indirect indexed with zero-page wrapping at $FF") {
      cpu.SetRegister(CPU::Register::A, 0xA5);
      cpu.SetRegister(CPU::Register::Y, 0x06);

      // Set up zero page wrapping: $FF/$00 -> base $0400, then add Y
      cpu.WriteToMemory(0x00FF, 0x00); // Low byte of base address
      cpu.WriteToMemory(0x0000, 0x04); // High byte (wraps to $00)
      cpu.WriteToMemory(0x0100, 0x00); // Ensure buggy code fails
      cpu.WriteToMemory(0x0406, 0x5A); // Value at $0400 + Y ($06)

      auto instr = CPU::ExclusiveOR<AddressingMode::IndirectY>{0xFF};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0xFF (0xA5 XOR 0x5A)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFF);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
    }

    WHEN("We execute an ORA immediate instruction") {
      cpu.SetRegister(CPU::Register::A, 0x0F);

      auto instr = CPU::BitwiseOR<AddressingMode::Immediate>{0xF0};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0xFF, with negative flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFF);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.SetRegister(CPU::Register::A, 0x00);
      instr.value = 0x00;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x00, with zero flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute an ORA zero-page instruction") {
      // Setup memory for zero-page test
      cpu.WriteToMemory(0x42, 0x0F);
      cpu.SetRegister(CPU::Register::A, 0xF0);

      auto instr = CPU::BitwiseOR<AddressingMode::ZeroPage>{0x42};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the OR result from memory") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFF);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x43, 0x55);
      cpu.SetRegister(CPU::Register::A, 0xAA);
      instr.value = 0x43;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0xFF, with negative flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFF);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute an ORA zero-page,X instruction") {
      // Setup memory and X register for zero-page,X test
      cpu.SetRegister(CPU::Register::X, 0x05);
      cpu.WriteToMemory(0x47, 0x3C); // 0x42 + 0x05 = 0x47
      cpu.SetRegister(CPU::Register::A, 0x03);

      auto instr = CPU::BitwiseOR<AddressingMode::ZeroPageX>{0x42};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the OR result from zero-page,X") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x3F);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute an ORA absolute instruction") {
      // Setup memory for absolute test
      cpu.WriteToMemory(0x0300, 0x80);
      cpu.SetRegister(CPU::Register::A, 0x01);

      auto instr = CPU::BitwiseOR<AddressingMode::Absolute>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the OR result from memory with negative flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x81);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute an ORA absolute,X instruction") {
      // Setup memory and X register for absolute,X test
      cpu.SetRegister(CPU::Register::X, 0x05);
      cpu.WriteToMemory(0x0305, 0x7E); // 0x0300 + 0x05 = 0x0305
      cpu.SetRegister(CPU::Register::A, 0x01);

      auto instr = CPU::BitwiseOR<AddressingMode::AbsoluteX>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the OR result from absolute,X") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x7F);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute an ORA absolute,Y instruction") {
      // Setup memory and Y register for absolute,Y test
      cpu.SetRegister(CPU::Register::Y, 0x05);
      cpu.WriteToMemory(0x0305, 0x7E); // 0x0300 + 0x05 = 0x0305
      cpu.SetRegister(CPU::Register::A, 0x01);

      auto instr = CPU::BitwiseOR<AddressingMode::AbsoluteY>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the OR result from absolute,Y") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x7F);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }

    WHEN("We execute an ORA indirect,X instruction") {
      cpu.SetRegister(CPU::Register::A, 0x0F);
      cpu.SetRegister(CPU::Register::X, 0x05);

      // Set up indirect addressing: pointer at 0x25 (0x20 + 0x05) points to 0x0300
      cpu.WriteToMemory(0x25, 0x00);   // Low byte of target address
      cpu.WriteToMemory(0x26, 0x03);   // High byte of target address
      cpu.WriteToMemory(0x0300, 0xF0); // Value at target address

      auto instr = CPU::BitwiseOR<AddressingMode::IndirectX>{0x20};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the OR result from indirect,X") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFF);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute ORA indexed indirect with zero-page wrapping at $FF") {
      cpu.SetRegister(CPU::Register::A, 0x0F);
      cpu.SetRegister(CPU::Register::X, 0x00);

      // Set up zero page wrapping: $FF/$00 -> $0400
      cpu.WriteToMemory(0x00FF, 0x00); // Low byte
      cpu.WriteToMemory(0x0000, 0x04); // High byte (wraps)
      cpu.WriteToMemory(0x0100, 0x00); // Ensure buggy code fails
      cpu.WriteToMemory(0x0400, 0xF0); // Value to OR

      auto instr = CPU::BitwiseOR<AddressingMode::IndirectX>{0xFF};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0xFF (0x0F OR 0xF0)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFF);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
    }

    WHEN("We execute an ORA indirect,Y instruction") {
      cpu.SetRegister(CPU::Register::A, 0x33);
      cpu.SetRegister(CPU::Register::Y, 0x05);

      // Set up indirect addressing: pointer at 0x20 points to 0x0300, then add Y
      cpu.WriteToMemory(0x20, 0x00);   // Low byte of base address
      cpu.WriteToMemory(0x21, 0x03);   // High byte of base address
      cpu.WriteToMemory(0x0305, 0xCC); // Value at target address (0x0300 + 0x05)

      auto instr = CPU::BitwiseOR<AddressingMode::IndirectY>{0x20};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the OR result from indirect,Y") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFF);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute ORA indirect indexed with zero-page wrapping at $FF") {
      cpu.SetRegister(CPU::Register::A, 0x3C);
      cpu.SetRegister(CPU::Register::Y, 0x07);

      // Set up zero page wrapping: $FF/$00 -> base $0400, then add Y
      cpu.WriteToMemory(0x00FF, 0x00); // Low byte of base address
      cpu.WriteToMemory(0x0000, 0x04); // High byte (wraps to $00)
      cpu.WriteToMemory(0x0100, 0x00); // Ensure buggy code fails
      cpu.WriteToMemory(0x0407, 0xC3); // Value at $0400 + Y ($07)

      auto instr = CPU::BitwiseOR<AddressingMode::IndirectY>{0xFF};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0xFF (0x3C OR 0xC3)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFF);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
    }

    WHEN("We execute CMP indexed indirect with zero-page wrapping at $FF") {
      cpu.SetRegister(CPU::Register::A, 0x5D);
      cpu.SetRegister(CPU::Register::X, 0x00);

      // Set up zero page wrapping: $FF/$00 -> $0400
      cpu.WriteToMemory(0x00FF, 0x00); // Low byte
      cpu.WriteToMemory(0x0000, 0x04); // High byte (wraps)
      cpu.WriteToMemory(0x0100, 0x00); // Ensure buggy code fails
      cpu.WriteToMemory(0x0400, 0x5D); // Value to compare (equal)

      auto instr = CPU::CompareRegister<CPU::Register::A, AddressingMode::IndirectX>{0xFF};
      cpu.RunInstruction(instr);

      THEN("The flags should indicate equality (0x5D == 0x5D)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x5D); // Accumulator unchanged
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);      // Equal
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);     // A >= M
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false); // Result is 0
      }
    }

    WHEN("We execute CMP indirect indexed with zero-page wrapping at $FF") {
      cpu.SetRegister(CPU::Register::A, 0x42);
      cpu.SetRegister(CPU::Register::Y, 0x01);

      // Set up zero page wrapping: $FF/$00 -> base $0400, then add Y
      cpu.WriteToMemory(0x00FF, 0x00); // Low byte of base address
      cpu.WriteToMemory(0x0000, 0x04); // High byte (wraps to $00)
      cpu.WriteToMemory(0x0100, 0x00); // Ensure buggy code fails
      cpu.WriteToMemory(0x0401, 0x30); // Value at $0400 + Y ($01)

      auto instr = CPU::CompareRegister<CPU::Register::A, AddressingMode::IndirectY>{0xFF};
      cpu.RunInstruction(instr);

      THEN("The flags should indicate A > M (0x42 > 0x30)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x42); // Accumulator unchanged
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);     // Not equal
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);     // A >= M
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false); // Result is positive
      }
    }

    WHEN("We execute CMP zero-page,X instruction") {
      cpu.SetRegister(CPU::Register::A, 0x50);
      cpu.SetRegister(CPU::Register::X, 0x10);
      cpu.WriteToMemory(0x30, 0x40); // Value at $20 + X ($10) = $30

      auto instr = CPU::CompareRegister<CPU::Register::A, AddressingMode::ZeroPageX>{0x20};
      cpu.RunInstruction(instr);

      THEN("The flags should indicate A > M (0x50 > 0x40)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x50); // Accumulator unchanged
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);     // Not equal
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);     // A >= M
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false); // Result is positive
      }
    }

    WHEN("We execute CMP absolute,X instruction") {
      cpu.SetRegister(CPU::Register::A, 0x80);
      cpu.SetRegister(CPU::Register::X, 0x05);
      cpu.WriteToMemory(0x0305, 0x80); // Value at $0300 + X ($05) = $0305

      auto instr = CPU::CompareRegister<CPU::Register::A, AddressingMode::AbsoluteX>{0x0300};
      cpu.RunInstruction(instr);

      THEN("The flags should indicate equality (0x80 == 0x80)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x80); // Accumulator unchanged
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);      // Equal
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);     // A >= M
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false); // Result is 0
      }
    }

    WHEN("We execute CMP absolute,Y instruction") {
      cpu.SetRegister(CPU::Register::A, 0x40);
      cpu.SetRegister(CPU::Register::Y, 0x00);
      cpu.WriteToMemory(0x0400, 0x40); // Value at $0400 + Y ($00) = $0400

      auto instr = CPU::CompareRegister<CPU::Register::A, AddressingMode::AbsoluteY>{0x0400};
      cpu.RunInstruction(instr);

      THEN("The flags should indicate equality (0x40 == 0x40)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x40); // Accumulator unchanged
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);      // Equal
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);     // A >= M
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false); // Result is 0
      }
    }

    WHEN("We execute a DCP zero page instruction") {
      cpu.SetRegister(CPU::Register::A, 0x50);
      cpu.WriteToMemory(0x0050, 0x30); // Memory contains 0x30

      auto dcp_instr = CPU::DecrementAndCompare<AddressingMode::ZeroPage>{0x50};
      cpu.RunInstruction(dcp_instr);

      THEN("Memory should be decremented to 0x2F and compared with A (0x50 - 0x2F = 0x21)") {
        REQUIRE(cpu.ReadFromMemory(0x0050) == 0x2F);        // Decremented
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x50); // Accumulator unchanged
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true); // 0x50 >= 0x2F
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
    }

    WHEN("We execute a DCP zero page,X instruction") {
      cpu.SetRegister(CPU::Register::A, 0x10);
      cpu.SetRegister(CPU::Register::X, 0x05);
      cpu.WriteToMemory(0x0055, 0x11); // Memory at $50 + X contains 0x11

      auto dcp_instr = CPU::DecrementAndCompare<AddressingMode::ZeroPageX>{0x50};
      cpu.RunInstruction(dcp_instr);

      THEN("Memory should be decremented to 0x10 and compared with A (equal)") {
        REQUIRE(cpu.ReadFromMemory(0x0055) == 0x10);
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x10);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true); // Equal
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
    }

    WHEN("We execute a DCP absolute instruction") {
      cpu.SetRegister(CPU::Register::A, 0x05);
      cpu.WriteToMemory(0x1234, 0x07); // Memory contains 0x07

      auto dcp_instr = CPU::DecrementAndCompare<AddressingMode::Absolute>{0x1234};
      cpu.RunInstruction(dcp_instr);

      THEN("Memory should be decremented to 0x06 and compared with A (0x05 < 0x06)") {
        REQUIRE(cpu.ReadFromMemory(0x1234) == 0x06);
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x05);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);   // A < M
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true); // 0x05 - 0x06 = 0xFF
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
      }
    }

    WHEN("We execute a DCP absolute,X instruction with underflow") {
      cpu.SetRegister(CPU::Register::A, 0xFF);
      cpu.SetRegister(CPU::Register::X, 0x10);
      cpu.WriteToMemory(0x1244, 0x01); // Memory at $1234 + X contains 0x01

      auto dcp_instr = CPU::DecrementAndCompare<AddressingMode::AbsoluteX>{0x1234};
      cpu.RunInstruction(dcp_instr);

      THEN("Memory should underflow to 0x00 and compared with A") {
        REQUIRE(cpu.ReadFromMemory(0x1244) == 0x00);
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFF);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);    // 0xFF >= 0x00
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true); // 0xFF - 0x00 = 0xFF
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
      }
    }

    WHEN("We execute a DCP absolute,Y instruction") {
      cpu.SetRegister(CPU::Register::A, 0x80);
      cpu.SetRegister(CPU::Register::Y, 0x08);
      cpu.WriteToMemory(0x0508, 0x81); // Memory at $0500 + Y contains 0x81

      auto dcp_instr = CPU::DecrementAndCompare<AddressingMode::AbsoluteY>{0x0500};
      cpu.RunInstruction(dcp_instr);

      THEN("Memory should be decremented to 0x80 and compared with A (equal)") {
        REQUIRE(cpu.ReadFromMemory(0x0508) == 0x80);
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x80);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
      }
    }

    WHEN("We execute a DCP (indirect,X) instruction") {
      cpu.SetRegister(CPU::Register::A, 0x20);
      cpu.SetRegister(CPU::Register::X, 0x0F);
      // Zero page $80 + X(0x0F) = $8F contains pointer to $0200
      cpu.WriteToMemory(0x008F, 0x00); // Low byte
      cpu.WriteToMemory(0x0090, 0x02); // High byte
      cpu.WriteToMemory(0x0200, 0x21); // Value at target address

      auto dcp_instr = CPU::DecrementAndCompare<AddressingMode::IndirectX>{0x80};
      cpu.RunInstruction(dcp_instr);

      THEN("Memory at indirect address should be decremented to 0x20 and compared") {
        REQUIRE(cpu.ReadFromMemory(0x0200) == 0x20);
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x20);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
    }

    WHEN("We execute a DCP (indirect),Y instruction") {
      cpu.SetRegister(CPU::Register::A, 0x42);
      cpu.SetRegister(CPU::Register::Y, 0x08);
      // Zero page $90 contains pointer to $0600
      cpu.WriteToMemory(0x0090, 0x00); // Low byte
      cpu.WriteToMemory(0x0091, 0x06); // High byte
      cpu.WriteToMemory(0x0608, 0x44); // Value at target + Y

      auto dcp_instr = CPU::DecrementAndCompare<AddressingMode::IndirectY>{0x90};
      cpu.RunInstruction(dcp_instr);

      THEN("Memory at (indirect) + Y should be decremented to 0x43 and compared") {
        REQUIRE(cpu.ReadFromMemory(0x0608) == 0x43);
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x42);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false);   // 0x42 < 0x43
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true); // 0x42 - 0x43 = 0xFF
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
    }

    WHEN("We execute an ISC zero page instruction") {
      cpu.SetRegister(CPU::Register::A, 0x50);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true); // No borrow
      cpu.WriteToMemory(0x0050, 0x2F);                      // Memory contains 0x2F

      auto isc_instr = CPU::IncrementAndSubtract<AddressingMode::ZeroPage>{0x50};
      cpu.RunInstruction(isc_instr);

      THEN("Memory should be incremented to 0x30, then subtracted from A (0x50 - 0x30 = 0x20)") {
        REQUIRE(cpu.ReadFromMemory(0x0050) == 0x30);        // Incremented
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x20); // Result of subtraction
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true); // No borrow
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
    }

    WHEN("We execute an ISC zero page,X instruction") {
      cpu.SetRegister(CPU::Register::A, 0x10);
      cpu.SetRegister(CPU::Register::X, 0x05);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.WriteToMemory(0x0055, 0x0F); // Memory at $50 + X contains 0x0F

      auto isc_instr = CPU::IncrementAndSubtract<AddressingMode::ZeroPageX>{0x50};
      cpu.RunInstruction(isc_instr);

      THEN("Memory should be incremented to 0x10, then subtracted from A (0x10 - 0x10 = 0)") {
        REQUIRE(cpu.ReadFromMemory(0x0055) == 0x10);
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);  // Result is zero
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true); // No borrow
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
    }

    WHEN("We execute an ISC absolute instruction with borrow") {
      cpu.SetRegister(CPU::Register::A, 0x05);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.WriteToMemory(0x1234, 0x06); // Memory contains 0x06

      auto isc_instr = CPU::IncrementAndSubtract<AddressingMode::Absolute>{0x1234};
      cpu.RunInstruction(isc_instr);

      THEN("Memory should be incremented to 0x07, then subtracted from A (0x05 - 0x07 = 0xFE)") {
        REQUIRE(cpu.ReadFromMemory(0x1234) == 0x07);
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFE);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false); // Borrow occurred
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
      }
    }

    WHEN("We execute an ISC absolute,X instruction with overflow") {
      cpu.SetRegister(CPU::Register::A, 0xFF);
      cpu.SetRegister(CPU::Register::X, 0x10);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.WriteToMemory(0x1244, 0xFF); // Memory at $1234 + X contains 0xFF

      auto isc_instr = CPU::IncrementAndSubtract<AddressingMode::AbsoluteX>{0x1234};
      cpu.RunInstruction(isc_instr);

      THEN("Memory should overflow to 0x00, then subtracted from A (0xFF - 0x00 = 0xFF)") {
        REQUIRE(cpu.ReadFromMemory(0x1244) == 0x00);
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFF);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
      }
    }

    WHEN("We execute an ISC absolute,Y instruction") {
      cpu.SetRegister(CPU::Register::A, 0x80);
      cpu.SetRegister(CPU::Register::Y, 0x08);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      cpu.WriteToMemory(0x0508, 0x7F); // Memory at $0500 + Y contains 0x7F

      auto isc_instr = CPU::IncrementAndSubtract<AddressingMode::AbsoluteY>{0x0500};
      cpu.RunInstruction(isc_instr);

      THEN("Memory should be incremented to 0x80, then subtracted from A (0x80 - 0x80 = 0)") {
        REQUIRE(cpu.ReadFromMemory(0x0508) == 0x80);
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
      }
    }

    WHEN("We execute an ISC (indirect,X) instruction") {
      cpu.SetRegister(CPU::Register::A, 0x21);
      cpu.SetRegister(CPU::Register::X, 0x0F);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      // Zero page $80 + X(0x0F) = $8F contains pointer to $0200
      cpu.WriteToMemory(0x008F, 0x00); // Low byte
      cpu.WriteToMemory(0x0090, 0x02); // High byte
      cpu.WriteToMemory(0x0200, 0x1F); // Value at target address

      auto isc_instr = CPU::IncrementAndSubtract<AddressingMode::IndirectX>{0x80};
      cpu.RunInstruction(isc_instr);

      THEN("Memory at indirect address should be incremented to 0x20, then subtracted (0x21 - 0x20 = 1)") {
        REQUIRE(cpu.ReadFromMemory(0x0200) == 0x20);
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x01);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
    }

    WHEN("We execute an ISC (indirect),Y instruction") {
      cpu.SetRegister(CPU::Register::A, 0x42);
      cpu.SetRegister(CPU::Register::Y, 0x08);
      cpu.SetStatusFlagValue(CPU::StatusFlag::Carry, true);
      // Zero page $90 contains pointer to $0600
      cpu.WriteToMemory(0x0090, 0x00); // Low byte
      cpu.WriteToMemory(0x0091, 0x06); // High byte
      cpu.WriteToMemory(0x0608, 0x42); // Value at target + Y

      auto isc_instr = CPU::IncrementAndSubtract<AddressingMode::IndirectY>{0x90};
      cpu.RunInstruction(isc_instr);

      THEN("Memory at (indirect) + Y should be incremented to 0x43, then subtracted (0x42 - 0x43 = 0xFF)") {
        REQUIRE(cpu.ReadFromMemory(0x0608) == 0x43);
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0xFF);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Carry) == false); // Borrow
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
    }
  }
}
