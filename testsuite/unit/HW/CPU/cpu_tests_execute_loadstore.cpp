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
  using CPU::WriteToMemory;
};

SCENARIO("6502 instruction execution tests (loads)", "[CPU][Loads]") {
  GIVEN("A freshly initialized cpu") {
    Bus bus;
    CPUMock cpu{bus};
    auto original_program_counter = cpu.ProgramCounter();

    WHEN("We execute a LDA immediate instruction") {
      auto instr = CPU::LoadRegister<CPU::Register::A, AddressingMode::Immediate>{0x42};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain the immediate value 0x42, with zero flag clear and negative flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      instr.value = 0x00;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x00, with zero flag set and negative flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      instr.value = 0x90;
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x90, with zero flag clear and negative flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a LDX immediate instruction") {
      auto instr = CPU::LoadRegister<CPU::Register::X, AddressingMode::Immediate>{0x42};
      cpu.RunInstruction(instr);

      THEN("The X register should contain the immediate value 0x42, with zero flag clear and negative flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      instr.value = 0x00;
      cpu.RunInstruction(instr);

      THEN("The X register should contain 0x00, with zero flag set and negative flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      instr.value = 0x90;
      cpu.RunInstruction(instr);

      THEN("The X register should contain 0x90, with zero flag clear and negative flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a LDY immediate instruction") {
      auto instr = CPU::LoadRegister<CPU::Register::Y, AddressingMode::Immediate>{0x42};
      cpu.RunInstruction(instr);

      THEN("The Y register should contain the immediate value 0x42, with zero flag clear and negative flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      instr.value = 0x00;
      cpu.RunInstruction(instr);

      THEN("The Y register should contain 0x00, with zero flag set and negative flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      instr.value = 0x90;
      cpu.RunInstruction(instr);

      THEN("The Y register should contain 0x90, with zero flag clear and negative flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a LDA zero-page instruction") {
      cpu.WriteToMemory(0x42, 0x22); // Write value 0x22 to zero-page address 0x42

      auto instr = CPU::LoadRegister<CPU::Register::A, AddressingMode::ZeroPage>{0x42};
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x22 from zero-page address 0x42, with zero flag clear and negative flag "
           "clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x0);
      cpu.RunInstruction(instr);

      THEN("The Y register should contain 0x90 from zero-page address 0x42, with zero flag clear and negative flag "
           "set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x0);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x90);
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x90 from zero-page address 0x42, with zero flag clear and negative flag "
           "set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
      original_program_counter = cpu.ProgramCounter();
    }

    WHEN("We execute a LDX zero-page instruction") {
      cpu.WriteToMemory(0x42, 0x22); // Write value 0x22 to zero-page address 0x42

      auto instr = CPU::LoadRegister<CPU::Register::X, AddressingMode::ZeroPage>{0x42};

      cpu.RunInstruction(instr);

      THEN("The X register should contain 0x22 from zero-page address 0x42, with zero flag clear and negative flag "
           "clear") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x0);
      cpu.RunInstruction(instr);

      THEN("The X register should contain 0x00 from zero-page address 0x42, with zero flag set and negative flag "
           "clear") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x0);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x90);
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x00 from zero-page address 0x42, with zero flag set and negative flag "
           "clear") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
      original_program_counter = cpu.ProgramCounter();
    }

    WHEN("We execute a LDY zero-page instruction") {
      cpu.WriteToMemory(0x42, 0x22); // Write value 0x22 to zero-page address 0x42

      auto instr = CPU::LoadRegister<CPU::Register::Y, AddressingMode::ZeroPage>{0x42};

      cpu.RunInstruction(instr);

      THEN("The Y register should contain 0x22 from zero-page address 0x42, with zero flag clear and negative flag "
           "clear") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x0);
      cpu.RunInstruction(instr);

      THEN("The Y register should contain 0x00 from zero-page address 0x42, with zero flag set and negative flag "
           "clear") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x90);
      cpu.RunInstruction(instr);

      THEN("The X register should contain 0x90 from zero-page address 0x42, with zero flag clear and negative flag "
           "set") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
      original_program_counter = cpu.ProgramCounter();
    }

    WHEN("We execute a LDA zero-page X instruction") {
      cpu.WriteToMemory(0x42, 0x22);           // Write value 0x22 to zero-page address 0x42
      cpu.SetRegister(CPU::Register::X, 0x02); // Set X register to 0x40

      auto instr = CPU::LoadRegister<CPU::Register::A, AddressingMode::ZeroPageX>{0x40};

      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x22 from zero-page address (0x40 + X), with zero flag clear and negative "
           "flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x00);
      cpu.RunInstruction(instr);

      THEN("The Y register should contain 0x22 from zero-page address (0x40 + X), with zero flag clear and negative "
           "flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x90);
      cpu.RunInstruction(instr);

      THEN("The Y register should contain 0x00 from zero-page address (0x40 + X), with zero flag set and negative flag "
           "clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a LDX zero-page Y instruction") {
      cpu.WriteToMemory(0x42, 0x22);           // Write value 0x22 to zero-page address 0x42
      cpu.SetRegister(CPU::Register::Y, 0x02); // Set Y register to 0x40

      auto instr = CPU::LoadRegister<CPU::Register::X, AddressingMode::ZeroPageY>{0x40};

      cpu.RunInstruction(instr);

      THEN("The X register should contain 0x90 from zero-page address (0x40 + Y), with zero flag clear and negative "
           "flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x00);
      cpu.RunInstruction(instr);

      THEN("The X register should contain 0x00 from zero-page address (0x40 + Y), with zero flag set and negative flag "
           "clear") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x90);
      cpu.RunInstruction(instr);

      THEN("The X register should contain 0x22 from zero-page address (0x40 + Y), with zero flag clear and negative "
           "flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a LDY zero-page X instruction") {
      cpu.WriteToMemory(0x42, 0x22);           // Write value 0x22 to zero-page address 0x42
      cpu.SetRegister(CPU::Register::X, 0x02); // Set X register to 0x40

      auto instr = CPU::LoadRegister<CPU::Register::Y, AddressingMode::ZeroPageX>{0x40};

      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x00 from zero-page address (0x40 + X), with zero flag set and negative "
           "flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x00);
      cpu.RunInstruction(instr);

      THEN("The Y register should contain 0x90 from zero-page address (0x40 + X), with zero flag clear and negative "
           "flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x90);
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x90 from zero-page address (0x40 + X), with zero flag clear and negative "
           "flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We try to execute a LDA absolute instruction") {
      cpu.WriteToMemory(0x0142, 0x22); // Write value 0x22 to absolute address 0x0142

      auto instr = CPU::LoadRegister<CPU::Register::A, AddressingMode::Absolute>{0x0142};

      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x22 from absolute address (0x0140 + X), with zero flag clear and negative "
           "flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x00);
      cpu.RunInstruction(instr);

      THEN("The X register should contain 0x90 from absolute address 0x0142, with zero flag clear and negative flag "
           "set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x90);
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x00 from absolute address 0x0142, with zero flag set and negative flag "
           "clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We try to execute a LDX absolute instruction") {
      cpu.WriteToMemory(0x0142, 0x22); // Write value 0x22 to absolute address 0x0142

      auto instr = CPU::LoadRegister<CPU::Register::X, AddressingMode::Absolute>{0x0142};

      cpu.RunInstruction(instr);

      THEN("The Y register should contain 0x22 from absolute address (0x0140 + X), with zero flag clear and negative "
           "flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x00);
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x90 from absolute address (0x0140 + X), with zero flag clear and negative "
           "flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x90);
      cpu.RunInstruction(instr);

      THEN("The Y register should contain 0x00 from absolute address 0x0142, with zero flag set and negative flag "
           "clear") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We try to execute a LDY absolute instruction") {
      cpu.WriteToMemory(0x0142, 0x22); // Write value 0x22 to absolute address 0x0142

      auto instr = CPU::LoadRegister<CPU::Register::Y, AddressingMode::Absolute>{0x0142};

      cpu.RunInstruction(instr);

      THEN("The Y register should contain 0x22 from absolute address 0x0142, with zero flag clear and negative flag "
           "clear") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x00);
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x90 from absolute address 0x0142, with zero flag clear and negative flag "
           "set") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x90);
      cpu.RunInstruction(instr);

      THEN("The Y register should contain 0x00 from absolute address (0x0140 + X), with zero flag set and negative "
           "flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We try to execute a LDA absolute X instruction") {
      cpu.SetRegister(CPU::Register::X, 0x02);
      cpu.WriteToMemory(0x0142, 0x22); // Write value 0x22 to absolute address 0x0142

      auto instr = CPU::LoadRegister<CPU::Register::A, AddressingMode::AbsoluteX>{0x0140};

      cpu.RunInstruction(instr);

      THEN("The X register should contain 0x22 from absolute address (0x0140 + Y), with zero flag clear and negative "
           "flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x00);
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x00 from absolute address (0x0140 + X), with zero flag set and negative "
           "flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x90);
      cpu.RunInstruction(instr);

      THEN("The X register should contain 0x00 from absolute address 0x0142, with zero flag set and negative flag "
           "clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We try to execute a LDA absolute Y instruction") {
      cpu.SetRegister(CPU::Register::Y, 0x02);
      cpu.WriteToMemory(0x0142, 0x22); // Write value 0x22 to absolute address 0x0142

      auto instr = CPU::LoadRegister<CPU::Register::A, AddressingMode::AbsoluteY>{0x0140};

      cpu.RunInstruction(instr);

      THEN("The X register should contain 0x22 from absolute address 0x0142, with zero flag clear and negative flag "
           "clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x00);
      cpu.RunInstruction(instr);

      THEN("The X register should contain 0x90 from absolute address (0x0140 + Y), with zero flag clear and negative "
           "flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x90);
      cpu.RunInstruction(instr);

      THEN("The X register should contain 0x00 from absolute address (0x0140 + Y), with zero flag set and negative "
           "flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We try to execute a LDX absolute Y instruction") {
      cpu.SetRegister(CPU::Register::Y, 0x02);
      cpu.WriteToMemory(0x0142, 0x22); // Write value 0x22 to absolute address 0x0142

      auto instr = CPU::LoadRegister<CPU::Register::X, AddressingMode::AbsoluteY>{0x0140};

      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x22 from absolute address (0x0140 + Y), with zero flag clear and negative "
           "flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x00);
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x90 from absolute address (0x0140 + Y), with zero flag clear and negative "
           "flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x90);
      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x00 from absolute address (0x0140 + Y), with zero flag set and negative "
           "flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::X] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }
    WHEN("We try to execute a LDY absolute X instruction") {
      cpu.SetRegister(CPU::Register::X, 0x02);
      cpu.WriteToMemory(0x0142, 0x22); // Write value 0x22 to absolute address 0x0142

      auto instr = CPU::LoadRegister<CPU::Register::Y, AddressingMode::AbsoluteX>{0x0140};

      cpu.RunInstruction(instr);

      THEN("The accumulator should contain 0x22 from absolute address 0x0142, with zero flag clear and negative flag "
           "clear") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x00);
      cpu.RunInstruction(instr);

      THEN("The Y register should contain 0x90 from absolute address (0x0140 + X), with zero flag clear and negative "
           "flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x90);
      cpu.RunInstruction(instr);

      THEN("The Y register should contain 0x90 from absolute address 0x0142, with zero flag clear and negative flag "
           "set") {
        REQUIRE(cpu.Registers()[CPU::Register::Y] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We try to execute a LDA indexed indirect instruction") {
      cpu.SetRegister(CPU::Register::X, 0x02);

      // the target memory address is 0x42, there we'll find the values {0x20, 0x04} which will lead the CPU to load the
      // content of memory address 0x420
      cpu.WriteToMemory(0x042, 0x20);
      cpu.WriteToMemory(0x043, 0x04);

      // then memory address 0x420 will contain the final value to load into the accumulator
      cpu.WriteToMemory(0x0420, 0x22);

      auto lda_instr = CPU::LoadRegister<CPU::Register::A, AddressingMode::IndirectX>{0x40};
      cpu.RunInstruction(lda_instr);

      THEN("The accumulator should contain 0x00 from address 0x0420 (indirect indexed via 0x42 + Y), with zero flag "
           "set and negative flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x420, 0x00);
      cpu.RunInstruction(lda_instr);

      THEN("The accumulator should contain 0x90 from address 0x0420 (indexed indirect via 0x40+X), with zero flag "
           "clear and negative flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x420, 0x90);
      cpu.RunInstruction(lda_instr);

      THEN("The accumulator should contain 0x22 from address 0x0420 (indexed indirect via 0x40+X), with zero flag "
           "clear and negative flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We try to execute a LDA indirect indexed instruction") {
      cpu.SetRegister(CPU::Register::Y, 0x02);

      // the target memory address is 0x42, there we'll find the values {0x20, 0x04} which will lead the CPU to load the
      // content of memory address 0x420
      cpu.WriteToMemory(0x042, 0x1E);
      cpu.WriteToMemory(0x043, 0x04);

      // then memory address 0x420 will contain the final value to load into the accumulator
      cpu.WriteToMemory(0x0420, 0x22);

      auto lda_instr = CPU::LoadRegister<CPU::Register::A, AddressingMode::IndirectY>{0x42};
      cpu.RunInstruction(lda_instr);

      THEN("The accumulator should contain 0x22 from address 0x0420 (indirect indexed via 0x42 + Y), with zero flag "
           "clear and negative flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x420, 0x00);
      cpu.RunInstruction(lda_instr);

      THEN("The accumulator should contain 0x00 from address 0x0420 (indexed indirect via 0x40+X), with zero flag set "
           "and negative flag clear") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x420, 0x90);
      cpu.RunInstruction(lda_instr);

      THEN("The accumulator should contain 0x90 from address 0x0420 (indirect indexed via 0x42 + Y), with zero flag "
           "clear and negative flag set") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute LDA indexed indirect with zero-page wrapping at $FF") {
      // This tests the specific bug found in nestest at line 1100
      // LDA ($FF,X) with X=0 should wrap the high byte read to $00
      cpu.SetRegister(CPU::Register::X, 0x00);

      // Set up zero page: $FF contains low byte, $00 (wrapping) contains high byte
      cpu.WriteToMemory(0x00FF, 0x00); // Low byte of target address
      cpu.WriteToMemory(0x0000, 0x04); // High byte of target address (wraps to $00)
      
      // Explicitly clear $100 to ensure the buggy code fails
      cpu.WriteToMemory(0x0100, 0x00);
      
      // Target address is $0400, write the value there
      cpu.WriteToMemory(0x0400, 0x5D);

      auto lda_instr = CPU::LoadRegister<CPU::Register::A, AddressingMode::IndirectX>{0xFF};
      cpu.RunInstruction(lda_instr);

      THEN("The accumulator should contain 0x5D from address 0x0400 (via wrapped zero-page pointer at $FF/$00)") {
        REQUIRE(cpu.Registers()[CPU::Register::A] == 0x5D);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(CPU::StatusFlag::Negative) == false);
      }
    }
  }
}

SCENARIO("6502 instruction execution tests (stores)", "[CPU][Stores]") {
  GIVEN("A freshly initialized cpu") {
    Bus bus;
    CPUMock cpu{bus};
    auto original_program_counter = cpu.ProgramCounter();

    WHEN("We execute a STA zero-page instruction") {
      cpu.SetRegister(CPU::Register::A, 0x22); // Set the accumulator to 0x22

      auto instr = CPU::StoreRegister<CPU::Register::A, AddressingMode::ZeroPage>{0x42};
      cpu.RunInstruction(instr);

      THEN("The memory address should contain 0x22 from the accumulator") {
        REQUIRE(cpu.ReadFromMemory(0x42) == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
      original_program_counter = cpu.ProgramCounter();
    }

    WHEN("We execute a STX zero-page instruction") {
      cpu.SetRegister(CPU::Register::X, 0x22);
      auto instr = CPU::StoreRegister<CPU::Register::X, AddressingMode::ZeroPage>{0x42};
      cpu.RunInstruction(instr);

      THEN("The memory address should contain 0x22 from the X register") {
        REQUIRE(cpu.ReadFromMemory(0x42) == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
      original_program_counter = cpu.ProgramCounter();
    }

    WHEN("We execute a STY zero-page instruction") {
      cpu.SetRegister(CPU::Register::Y, 0x22);

      auto instr = CPU::StoreRegister<CPU::Register::Y, AddressingMode::ZeroPage>{0x42};
      cpu.RunInstruction(instr);

      THEN("The memory address should contain 0x22 from the accumulator") {
        REQUIRE(cpu.ReadFromMemory(0x42) == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
      original_program_counter = cpu.ProgramCounter();
    }

    WHEN("We execute a STA zero-page X instruction") {
      cpu.SetRegister(CPU::Register::A, 0x22);
      cpu.SetRegister(CPU::Register::X, 0x02);

      auto instr = CPU::StoreRegister<CPU::Register::A, AddressingMode::ZeroPageX>{0x40};
      cpu.RunInstruction(instr);

      THEN("The memory address should contain 0x22 from the accumulator") {
        REQUIRE(cpu.ReadFromMemory(0x42) == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
      original_program_counter = cpu.ProgramCounter();
    }

    WHEN("We execute a STX zero-page Y instruction") {
      cpu.SetRegister(CPU::Register::X, 0x22);
      cpu.SetRegister(CPU::Register::Y, 0x02);

      auto instr = CPU::StoreRegister<CPU::Register::X, AddressingMode::ZeroPageY>{0x40};
      cpu.RunInstruction(instr);

      THEN("The memory address should contain 0x22 from the accumulator") {
        REQUIRE(cpu.ReadFromMemory(0x42) == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
      original_program_counter = cpu.ProgramCounter();
    }

    WHEN("We execute a STY zero-page X instruction") {
      cpu.SetRegister(CPU::Register::Y, 0x22);
      cpu.SetRegister(CPU::Register::X, 0x02);

      auto instr = CPU::StoreRegister<CPU::Register::Y, AddressingMode::ZeroPageX>{0x40};
      cpu.RunInstruction(instr);

      THEN("The memory address should contain 0x22 from the accumulator") {
        REQUIRE(cpu.ReadFromMemory(0x42) == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
      original_program_counter = cpu.ProgramCounter();
    }

    WHEN("We execute a STA absolute instruction") {
      cpu.SetRegister(CPU::Register::A, 0x22); // Set the accumulator to 0x22

      auto instr = CPU::StoreRegister<CPU::Register::A, AddressingMode::Absolute>{0x142};
      cpu.RunInstruction(instr);

      THEN("The memory address should contain 0x22 from the accumulator") {
        REQUIRE(cpu.ReadFromMemory(0x142) == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
      }
      original_program_counter = cpu.ProgramCounter();
    }

    WHEN("We execute a STX absolute instruction") {
      cpu.SetRegister(CPU::Register::X, 0x22);

      auto instr = CPU::StoreRegister<CPU::Register::X, AddressingMode::Absolute>{0x142};
      cpu.RunInstruction(instr);

      THEN("The memory address should contain 0x22 from the X register") {
        REQUIRE(cpu.ReadFromMemory(0x142) == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
      }
      original_program_counter = cpu.ProgramCounter();
    }

    WHEN("We execute a STY absolute instruction") {
      cpu.SetRegister(CPU::Register::Y, 0x22);

      auto instr = CPU::StoreRegister<CPU::Register::Y, AddressingMode::Absolute>{0x142};
      cpu.RunInstruction(instr);

      THEN("The memory address should contain 0x22 from the accumulator") {
        REQUIRE(cpu.ReadFromMemory(0x142) == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
      }
      original_program_counter = cpu.ProgramCounter();
    }

    WHEN("We execute a STA absolute X instruction") {
      cpu.SetRegister(CPU::Register::A, 0x22); // Set the accumulator to 0x22
      cpu.SetRegister(CPU::Register::X, 0x02);

      auto instr = CPU::StoreRegister<CPU::Register::A, AddressingMode::AbsoluteX>{0x140};
      cpu.RunInstruction(instr);

      THEN("The memory address should contain 0x22 from the accumulator") {
        REQUIRE(cpu.ReadFromMemory(0x142) == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
      }
      original_program_counter = cpu.ProgramCounter();
    }

    WHEN("We execute a STA absolute Y instruction") {
      cpu.SetRegister(CPU::Register::A, 0x22); // Set the accumulator to 0x22
      cpu.SetRegister(CPU::Register::Y, 0x02);

      auto instr = CPU::StoreRegister<CPU::Register::A, AddressingMode::AbsoluteY>{0x140};
      cpu.RunInstruction(instr);

      THEN("The memory address should contain 0x22 from the accumulator") {
        REQUIRE(cpu.ReadFromMemory(0x142) == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
      }
      original_program_counter = cpu.ProgramCounter();
    }

    WHEN("We execute a STA indirect X instruction") {
      cpu.SetRegister(CPU::Register::A, 0x22); // Set the accumulator to 0x22
      cpu.SetRegister(CPU::Register::X, 0x04); // Set X register to 0x04

      // Set up the indirect address table at zero page address 0x20 + X (0x24)
      // The target address will be stored at 0x24 (LSB) and 0x25 (MSB)
      cpu.WriteToMemory(0x24, 0x00); // LSB of target address (0x0300)
      cpu.WriteToMemory(0x25, 0x03); // MSB of target address (0x0300)

      auto instr = CPU::StoreRegister<CPU::Register::A, AddressingMode::IndirectX>{0x20};
      cpu.RunInstruction(instr);

      THEN("The memory address should contain 0x22 from the accumulator") {
        REQUIRE(cpu.ReadFromMemory(0x0300) == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
      original_program_counter = cpu.ProgramCounter();
    }

    WHEN("We execute STA indexed indirect with zero-page wrapping at $FF") {
      // Test zero-page wrapping for STA (Indirect,X)
      cpu.SetRegister(CPU::Register::A, 0xAB); // Value to store
      cpu.SetRegister(CPU::Register::X, 0x00);

      // Set up zero page: $FF contains low byte, $00 (wrapping) contains high byte
      cpu.WriteToMemory(0x00FF, 0x00); // Low byte of target address
      cpu.WriteToMemory(0x0000, 0x05); // High byte of target address (wraps to $00)
      cpu.WriteToMemory(0x0100, 0x00); // Ensure buggy code would fail

      auto sta_instr = CPU::StoreRegister<CPU::Register::A, AddressingMode::IndirectX>{0xFF};
      cpu.RunInstruction(sta_instr);

      THEN("The value should be stored at address 0x0500 (via wrapped zero-page pointer at $FF/$00)") {
        REQUIRE(cpu.ReadFromMemory(0x0500) == 0xAB);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
    }

    WHEN("We execute a STA indirect Y instruction") {
      cpu.SetRegister(CPU::Register::A, 0x22); // Set the accumulator to 0x22
      cpu.SetRegister(CPU::Register::Y, 0x10); // Set Y register to 0x10

      // Set up the base address at zero page address 0x30
      // The base address will be stored at 0x30 (LSB) and 0x31 (MSB)
      cpu.WriteToMemory(0x30, 0x00); // LSB of base address (0x0200)
      cpu.WriteToMemory(0x31, 0x02); // MSB of base address (0x0200)
      // Final address will be 0x0200 + Y (0x10) = 0x0210

      auto instr = CPU::StoreRegister<CPU::Register::A, AddressingMode::IndirectY>{0x30};
      cpu.RunInstruction(instr);

      THEN("The memory address should contain 0x22 from the accumulator") {
        REQUIRE(cpu.ReadFromMemory(0x0210) == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
      }
      original_program_counter = cpu.ProgramCounter();
    }
  }
}
