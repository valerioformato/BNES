//
// Created by vformato on 7/22/25.
//

#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

template <size_t N> using ByteArray = std::array<uint8_t, N>;

class CPUMock : public BNES::HW::CPU {
  // This mock class is used to expose the WriteToMemory method for testing purposes.
  // In a real scenario, you would not need this as CPU would be used directly.
public:
  using CPU::SetRegister;
  using CPU::WriteToMemory;
};

SCENARIO("6502 instruction execution tests (loads and stores)") {
  GIVEN("A freshly initialized cpu") {
    CPUMock cpu;
    auto original_program_counter = cpu.ProgramCounter();

    WHEN("We execute a LDA immediate instruction") {
      auto program = ByteArray<2>{0xA9, 0x42}; // LDA #$42
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the immediate value") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      program[1] = 0x00;
      instr = cpu.DecodeInstruction(program);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      program[1] = 0x90;
      instr = cpu.DecodeInstruction(program);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a LDX immediate instruction") {
      auto program = ByteArray<2>{0xA2, 0x42};
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the immediate value") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      program[1] = 0x00;
      instr = cpu.DecodeInstruction(program);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      program[1] = 0x90;
      instr = cpu.DecodeInstruction(program);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a LDY immediate instruction") {
      auto program = ByteArray<2>{0xA0, 0x42};
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the immediate value") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::Y] == 0x42);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      program[1] = 0x00;
      instr = cpu.DecodeInstruction(program);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::Y] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      program[1] = 0x90;
      instr = cpu.DecodeInstruction(program);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be zero and the zero flag should be set") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::Y] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a LDA zero page instruction") {
      cpu.WriteToMemory(0x42, 0x22); // Write value 0x22 to zero page address 0x42

      auto program = ByteArray<2>{0xA5, 0x42}; // LDA $42
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x0);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x0);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x90);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
      original_program_counter = cpu.ProgramCounter();
    }

    WHEN("We execute a LDX zero page instruction") {
      cpu.WriteToMemory(0x42, 0x22); // Write value 0x22 to zero page address 0x42

      auto program = ByteArray<2>{0xA6, 0x42}; // LDX $42
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x0);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x0);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x90);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
      original_program_counter = cpu.ProgramCounter();
    }

    WHEN("We execute a LDY zero page instruction") {
      cpu.WriteToMemory(0x42, 0x22); // Write value 0x22 to zero page address 0x42

      auto program = ByteArray<2>{0xA4, 0x42}; // LDY $42
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::Y] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x0);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::Y] == 0x0);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x90);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::Y] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
      original_program_counter = cpu.ProgramCounter();
    }

    WHEN("We execute a LDA zero page X instruction") {
      cpu.WriteToMemory(0x42, 0x22);                     // Write value 0x22 to zero page address 0x42
      cpu.SetRegister(BNES::HW::CPU::Register::X, 0x02); // Set X register to 0x40

      auto program = ByteArray<2>{0xB5, 0x40}; // LDA $40,X
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page + X") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x00);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page + X") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x90);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page + X") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a LDX zero page Y instruction") {
      cpu.WriteToMemory(0x42, 0x22);                     // Write value 0x22 to zero page address 0x42
      cpu.SetRegister(BNES::HW::CPU::Register::Y, 0x02); // Set Y register to 0x40

      auto program = ByteArray<2>{0xB6, 0x40}; // LDX $40,X
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page + Y") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x00);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page + Y") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x90);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page + Y") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We execute a LDY zero page X instruction") {
      cpu.WriteToMemory(0x42, 0x22);                     // Write value 0x22 to zero page address 0x42
      cpu.SetRegister(BNES::HW::CPU::Register::X, 0x02); // Set X register to 0x40

      auto program = ByteArray<2>{0xB4, 0x40}; // LDY $40,X
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page + X") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::Y] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x00);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page + X") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::Y] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x42, 0x90);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from zero page + X") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::Y] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We try to execute a LDA absolute instruction") {
      cpu.WriteToMemory(0x0142, 0x22); // Write value 0x22 to absolute address 0x0142

      auto program = ByteArray<3>{0xAD, 0x42, 0x01}; // LDA $0142
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from absolute address") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x00);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from memory + X") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x90);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from memory + X") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We try to execute a LDX absolute instruction") {
      cpu.WriteToMemory(0x0142, 0x22); // Write value 0x22 to absolute address 0x0142

      auto program = ByteArray<3>{0xAE, 0x42, 0x01}; // LDX $0142
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from absolute address") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x00);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from memory + X") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x90);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from memory + X") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We try to execute a LDY absolute instruction") {
      cpu.WriteToMemory(0x0142, 0x22); // Write value 0x22 to absolute address 0x0142

      auto program = ByteArray<3>{0xAC, 0x42, 0x01}; // LDY $0142
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from absolute address") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::Y] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x00);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from memory + X") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::Y] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x90);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from memory + X") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::Y] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We try to execute a LDA absolute X instruction") {
      cpu.SetRegister(BNES::HW::CPU::Register::X, 0x02);
      cpu.WriteToMemory(0x0142, 0x22); // Write value 0x22 to absolute address 0x0142

      auto program = ByteArray<3>{0xBD, 0x40, 0x01}; // LDA $0140,X
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from absolute address") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x00);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from memory + X") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x90);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from memory + X") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We try to execute a LDA absolute Y instruction") {
      cpu.SetRegister(BNES::HW::CPU::Register::Y, 0x02);
      cpu.WriteToMemory(0x0142, 0x22); // Write value 0x22 to absolute address 0x0142

      auto program = ByteArray<3>{0xB9, 0x40, 0x01}; // LDA $0140,Y
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from absolute address") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x00);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from memory + Y") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x90);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from memory + Y") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We try to execute a LDX absolute Y instruction") {
      cpu.SetRegister(BNES::HW::CPU::Register::Y, 0x02);
      cpu.WriteToMemory(0x0142, 0x22); // Write value 0x22 to absolute address 0x0142

      auto program = ByteArray<3>{0xBE, 0x40, 0x01}; // LDX $0140,Y
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from absolute address") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x00);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from memory + Y") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x90);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from memory + Y") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::X] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
    }
    WHEN("We try to execute a LDY absolute X instruction") {
      cpu.SetRegister(BNES::HW::CPU::Register::X, 0x02);
      cpu.WriteToMemory(0x0142, 0x22); // Write value 0x22 to absolute address 0x0142

      auto program = ByteArray<3>{0xBC, 0x40, 0x01}; // LDY $0140,X
      auto instr = cpu.DecodeInstruction(program);

      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from absolute address") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::Y] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }

      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x00);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from memory + X") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::Y] == 0x00);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == true);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
      original_program_counter = cpu.ProgramCounter();

      cpu.WriteToMemory(0x142, 0x90);
      cpu.RunInstruction(instr);

      THEN("The accumulator should be loaded with the value from memory + X") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::Y] == 0x90);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 3);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == true);
      }
    }

    WHEN("We try to execute a LDA indexed indirect instruction") {
      cpu.SetRegister(BNES::HW::CPU::Register::X, 0x02);

      // the target memory address is 0x42, there we'll find the values {0x20, 0x04} which will lead the CPU to load the
      // content of memory address 0x420
      cpu.WriteToMemory(0x042, 0x20);
      cpu.WriteToMemory(0x043, 0x04);

      // then memory address 0x420 will contain the final value to load into the accumulator
      cpu.WriteToMemory(0x0420, 0x22);

      auto program = ByteArray<2>{0xA1, 0x40};
      auto lda_instr = cpu.DecodeInstruction(program);
      cpu.RunInstruction(lda_instr);

      THEN("The accumulator should be loaded with the correct value") {
        REQUIRE(cpu.Registers()[BNES::HW::CPU::Register::A] == 0x22);
        REQUIRE(cpu.ProgramCounter() == original_program_counter + 2);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Zero) == false);
        REQUIRE(cpu.TestStatusFlag(BNES::HW::CPU::StatusFlag::Negative) == false);
      }
    }
  }
}
