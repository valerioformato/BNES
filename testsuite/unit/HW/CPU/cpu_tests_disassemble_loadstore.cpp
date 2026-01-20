//
// Created by Valerio Formato on 09-Jan-26.
//

#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

using namespace BNES::HW;

SCENARIO("6502 instruction disassembly tests (load/store)", "[Disassemble][Load/Store]") {
  GIVEN("A freshly initialized cpu") {
    Bus bus;
    CPU cpu{bus};

    // LDA instructions
    WHEN("We disassemble a LDA immediate instruction") {
      auto instruction = CPU::LoadRegister<CPU::Register::A, AddressingMode::Immediate>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LDA #$42'") { REQUIRE(disassembly == "LDA #$42"); }
    }

    WHEN("We disassemble a LDA zero-page instruction") {
      auto instruction = CPU::LoadRegister<CPU::Register::A, AddressingMode::ZeroPage>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LDA $42'") { REQUIRE(disassembly == "LDA $42"); }
    }

    WHEN("We disassemble a LDA zero-page,X instruction") {
      auto instruction = CPU::LoadRegister<CPU::Register::A, AddressingMode::ZeroPageX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LDA $42,X'") { REQUIRE(disassembly == "LDA $42,X"); }
    }

    WHEN("We disassemble a LDA absolute instruction") {
      auto instruction = CPU::LoadRegister<CPU::Register::A, AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LDA $1234'") { REQUIRE(disassembly == "LDA $1234"); }
    }

    WHEN("We disassemble a LDA absolute,X instruction") {
      auto instruction = CPU::LoadRegister<CPU::Register::A, AddressingMode::AbsoluteX>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LDA $1234,X'") { REQUIRE(disassembly == "LDA $1234,X"); }
    }

    WHEN("We disassemble a LDA absolute,Y instruction") {
      auto instruction = CPU::LoadRegister<CPU::Register::A, AddressingMode::AbsoluteY>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LDA $1234,Y'") { REQUIRE(disassembly == "LDA $1234,Y"); }
    }

    WHEN("We disassemble a LDA (indirect,X) instruction") {
      auto instruction = CPU::LoadRegister<CPU::Register::A, AddressingMode::IndirectX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LDA ($42,X)'") { REQUIRE(disassembly == "LDA ($42,X)"); }
    }

    WHEN("We disassemble a LDA (indirect),Y instruction") {
      auto instruction = CPU::LoadRegister<CPU::Register::A, AddressingMode::IndirectY>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LDA ($42),Y'") { REQUIRE(disassembly == "LDA ($42),Y"); }
    }

    // LDX instructions
    WHEN("We disassemble a LDX immediate instruction") {
      auto instruction = CPU::LoadRegister<CPU::Register::X, AddressingMode::Immediate>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LDX #$42'") { REQUIRE(disassembly == "LDX #$42"); }
    }

    WHEN("We disassemble a LDX zero-page instruction") {
      auto instruction = CPU::LoadRegister<CPU::Register::X, AddressingMode::ZeroPage>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LDX $42'") { REQUIRE(disassembly == "LDX $42"); }
    }

    WHEN("We disassemble a LDX zero-page,Y instruction") {
      auto instruction = CPU::LoadRegister<CPU::Register::X, AddressingMode::ZeroPageY>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LDX $42,Y'") { REQUIRE(disassembly == "LDX $42,Y"); }
    }

    WHEN("We disassemble a LDX absolute instruction") {
      auto instruction = CPU::LoadRegister<CPU::Register::X, AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LDX $1234'") { REQUIRE(disassembly == "LDX $1234"); }
    }

    WHEN("We disassemble a LDX absolute,Y instruction") {
      auto instruction = CPU::LoadRegister<CPU::Register::X, AddressingMode::AbsoluteY>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LDX $1234,Y'") { REQUIRE(disassembly == "LDX $1234,Y"); }
    }

    // LDY instructions
    WHEN("We disassemble a LDY immediate instruction") {
      auto instruction = CPU::LoadRegister<CPU::Register::Y, AddressingMode::Immediate>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LDY #$42'") { REQUIRE(disassembly == "LDY #$42"); }
    }

    WHEN("We disassemble a LDY zero-page instruction") {
      auto instruction = CPU::LoadRegister<CPU::Register::Y, AddressingMode::ZeroPage>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LDY $42'") { REQUIRE(disassembly == "LDY $42"); }
    }

    WHEN("We disassemble a LDY zero-page,X instruction") {
      auto instruction = CPU::LoadRegister<CPU::Register::Y, AddressingMode::ZeroPageX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LDY $42,X'") { REQUIRE(disassembly == "LDY $42,X"); }
    }

    WHEN("We disassemble a LDY absolute instruction") {
      auto instruction = CPU::LoadRegister<CPU::Register::Y, AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LDY $1234'") { REQUIRE(disassembly == "LDY $1234"); }
    }

    WHEN("We disassemble a LDY absolute,X instruction") {
      auto instruction = CPU::LoadRegister<CPU::Register::Y, AddressingMode::AbsoluteX>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'LDY $1234,X'") { REQUIRE(disassembly == "LDY $1234,X"); }
    }

    // STA instructions
    WHEN("We disassemble a STA zero-page instruction") {
      auto instruction = CPU::StoreRegister<CPU::Register::A, AddressingMode::ZeroPage>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'STA $42'") { REQUIRE(disassembly == "STA $42"); }
    }

    WHEN("We disassemble a STA zero-page,X instruction") {
      auto instruction = CPU::StoreRegister<CPU::Register::A, AddressingMode::ZeroPageX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'STA $42,X'") { REQUIRE(disassembly == "STA $42,X"); }
    }

    WHEN("We disassemble a STA absolute instruction") {
      auto instruction = CPU::StoreRegister<CPU::Register::A, AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'STA $1234'") { REQUIRE(disassembly == "STA $1234"); }
    }

    WHEN("We disassemble a STA absolute,X instruction") {
      auto instruction = CPU::StoreRegister<CPU::Register::A, AddressingMode::AbsoluteX>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'STA $1234,X'") { REQUIRE(disassembly == "STA $1234,X"); }
    }

    WHEN("We disassemble a STA absolute,Y instruction") {
      auto instruction = CPU::StoreRegister<CPU::Register::A, AddressingMode::AbsoluteY>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'STA $1234,Y'") { REQUIRE(disassembly == "STA $1234,Y"); }
    }

    WHEN("We disassemble a STA (indirect,X) instruction") {
      auto instruction = CPU::StoreRegister<CPU::Register::A, AddressingMode::IndirectX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'STA ($42,X)'") { REQUIRE(disassembly == "STA ($42,X)"); }
    }

    WHEN("We disassemble a STA (indirect),Y instruction") {
      auto instruction = CPU::StoreRegister<CPU::Register::A, AddressingMode::IndirectY>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'STA ($42),Y'") { REQUIRE(disassembly == "STA ($42),Y"); }
    }

    // STX instructions
    WHEN("We disassemble a STX zero-page instruction") {
      auto instruction = CPU::StoreRegister<CPU::Register::X, AddressingMode::ZeroPage>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'STX $42'") { REQUIRE(disassembly == "STX $42"); }
    }

    WHEN("We disassemble a STX zero-page,Y instruction") {
      auto instruction = CPU::StoreRegister<CPU::Register::X, AddressingMode::ZeroPageY>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'STX $42,Y'") { REQUIRE(disassembly == "STX $42,Y"); }
    }

    WHEN("We disassemble a STX absolute instruction") {
      auto instruction = CPU::StoreRegister<CPU::Register::X, AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'STX $1234'") { REQUIRE(disassembly == "STX $1234"); }
    }

    // STY instructions
    WHEN("We disassemble a STY zero-page instruction") {
      auto instruction = CPU::StoreRegister<CPU::Register::Y, AddressingMode::ZeroPage>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'STY $42'") { REQUIRE(disassembly == "STY $42"); }
    }

    WHEN("We disassemble a STY zero-page,X instruction") {
      auto instruction = CPU::StoreRegister<CPU::Register::Y, AddressingMode::ZeroPageX>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'STY $42,X'") { REQUIRE(disassembly == "STY $42,X"); }
    }

    WHEN("We disassemble a STY absolute instruction") {
      auto instruction = CPU::StoreRegister<CPU::Register::Y, AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'STY $1234'") { REQUIRE(disassembly == "STY $1234"); }
    }

    WHEN("We disassemble a LAX zero page instruction") {
      auto instruction = CPU::LoadAccumulatorAndX<AddressingMode::ZeroPage>{0x50};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to '*LAX $50'") { REQUIRE(disassembly == "*LAX $50"); }
    }

    WHEN("We disassemble a LAX zero page,Y instruction") {
      auto instruction = CPU::LoadAccumulatorAndX<AddressingMode::ZeroPageY>{0x60};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to '*LAX $60,Y'") { REQUIRE(disassembly == "*LAX $60,Y"); }
    }

    WHEN("We disassemble a LAX absolute instruction") {
      auto instruction = CPU::LoadAccumulatorAndX<AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to '*LAX $1234'") { REQUIRE(disassembly == "*LAX $1234"); }
    }

    WHEN("We disassemble a LAX absolute,Y instruction") {
      auto instruction = CPU::LoadAccumulatorAndX<AddressingMode::AbsoluteY>{0xABCD};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to '*LAX $ABCD,Y'") { REQUIRE(disassembly == "*LAX $ABCD,Y"); }
    }

    WHEN("We disassemble a LAX (indirect,X) instruction") {
      auto instruction = CPU::LoadAccumulatorAndX<AddressingMode::IndirectX>{0x80};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to '*LAX ($80,X)'") { REQUIRE(disassembly == "*LAX ($80,X)"); }
    }

    WHEN("We disassemble a LAX (indirect),Y instruction") {
      auto instruction = CPU::LoadAccumulatorAndX<AddressingMode::IndirectY>{0x90};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to '*LAX ($90),Y'") { REQUIRE(disassembly == "*LAX ($90),Y"); }
    }
  }
}
