//
// Created by Valerio Formato on 09-Jan-26.
//

#include "HW/CPU.h"

#include <catch2/catch_test_macros.hpp>

using namespace BNES::HW;

SCENARIO("6502 instruction disassembly tests (general)", "[Disassemble]") {
  GIVEN("A freshly initialized cpu") {
    Bus bus;
    CPU cpu{bus};

    WHEN("We disassemble a BRK instruction") {
      auto instruction = CPU::Break{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'BRK'") { REQUIRE(disassembly == "BRK"); }
    }

    WHEN("We disassemble a NOP instruction") {
      auto instruction = CPU::NoOperation{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'NOP'") { REQUIRE(disassembly == "NOP"); }
    }

    WHEN("We disassemble a TAX instruction") {
      auto instruction = CPU::TransferRegisterTo<CPU::Register::A, CPU::Register::X>{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'TAX'") { REQUIRE(disassembly == "TAX"); }
    }

    WHEN("We disassemble a TAY instruction") {
      auto instruction = CPU::TransferRegisterTo<CPU::Register::A, CPU::Register::Y>{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'TAY'") { REQUIRE(disassembly == "TAY"); }
    }

    WHEN("We disassemble a TXA instruction") {
      auto instruction = CPU::TransferRegisterTo<CPU::Register::X, CPU::Register::A>{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'TXA'") { REQUIRE(disassembly == "TXA"); }
    }

    WHEN("We disassemble a TYA instruction") {
      auto instruction = CPU::TransferRegisterTo<CPU::Register::Y, CPU::Register::A>{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'TYA'") { REQUIRE(disassembly == "TYA"); }
    }

    WHEN("We disassemble a TSX instruction") {
      auto instruction = CPU::TransferStackPointerToX{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'TSX'") { REQUIRE(disassembly == "TSX"); }
    }

    WHEN("We disassemble a TXS instruction") {
      auto instruction = CPU::TransferXToStackPointer{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'TXS'") { REQUIRE(disassembly == "TXS"); }
    }

    WHEN("We disassemble a PHA instruction") {
      auto instruction = CPU::PushAccumulator{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'PHA'") { REQUIRE(disassembly == "PHA"); }
    }

    WHEN("We disassemble a PLA instruction") {
      auto instruction = CPU::PullAccumulator{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'PLA'") { REQUIRE(disassembly == "PLA"); }
    }

    WHEN("We disassemble a PHP instruction") {
      auto instruction = CPU::PushStatusRegister{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'PHP'") { REQUIRE(disassembly == "PHP"); }
    }

    WHEN("We disassemble a PLP instruction") {
      auto instruction = CPU::PullStatusRegister{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'PLP'") { REQUIRE(disassembly == "PLP"); }
    }

    WHEN("We disassemble a CPX immediate instruction") {
      auto instruction = CPU::CompareRegister<CPU::Register::X, AddressingMode::Immediate>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'CPX #$42'") { REQUIRE(disassembly == "CPX #$42"); }
    }

    WHEN("We disassemble a CPX zero-page instruction") {
      auto instruction = CPU::CompareRegister<CPU::Register::X, AddressingMode::ZeroPage>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'CPX $42'") { REQUIRE(disassembly == "CPX $42"); }
    }

    WHEN("We disassemble a CPX absolute instruction") {
      auto instruction = CPU::CompareRegister<CPU::Register::X, AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'CPX $1234'") { REQUIRE(disassembly == "CPX $1234"); }
    }

    WHEN("We disassemble a CPY immediate instruction") {
      auto instruction = CPU::CompareRegister<CPU::Register::Y, AddressingMode::Immediate>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'CPY #$42'") { REQUIRE(disassembly == "CPY #$42"); }
    }

    WHEN("We disassemble a CPY zero-page instruction") {
      auto instruction = CPU::CompareRegister<CPU::Register::Y, AddressingMode::ZeroPage>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'CPY $42'") { REQUIRE(disassembly == "CPY $42"); }
    }

    WHEN("We disassemble a CPY absolute instruction") {
      auto instruction = CPU::CompareRegister<CPU::Register::Y, AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'CPY $1234'") { REQUIRE(disassembly == "CPY $1234"); }
    }

    WHEN("We disassemble a BIT zero-page instruction") {
      auto instruction = CPU::BitTest<AddressingMode::ZeroPage>{0x42};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'BIT $42'") { REQUIRE(disassembly == "BIT $42"); }
    }

    WHEN("We disassemble a BIT absolute instruction") {
      auto instruction = CPU::BitTest<AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'BIT $1234'") { REQUIRE(disassembly == "BIT $1234"); }
    }

    WHEN("We disassemble a BNE instruction") {
      auto instruction = CPU::Branch<Conditional::NotEqual>{0x10};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'BNE $10'") { REQUIRE(disassembly == "BNE $10"); }
    }

    WHEN("We disassemble a BEQ instruction") {
      auto instruction = CPU::Branch<Conditional::Equal>{0x10};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'BEQ $10'") { REQUIRE(disassembly == "BEQ $10"); }
    }

    WHEN("We disassemble a BCC instruction") {
      auto instruction = CPU::Branch<Conditional::CarryClear>{0x10};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'BCC $10'") { REQUIRE(disassembly == "BCC $10"); }
    }

    WHEN("We disassemble a BCS instruction") {
      auto instruction = CPU::Branch<Conditional::CarrySet>{0x10};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'BCS $10'") { REQUIRE(disassembly == "BCS $10"); }
    }

    WHEN("We disassemble a BMI instruction") {
      auto instruction = CPU::Branch<Conditional::Minus>{0x10};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'BMI $10'") { REQUIRE(disassembly == "BMI $10"); }
    }

    WHEN("We disassemble a BPL instruction") {
      auto instruction = CPU::Branch<Conditional::Positive>{0x10};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'BPL $10'") { REQUIRE(disassembly == "BPL $10"); }
    }

    WHEN("We disassemble a BVC instruction") {
      auto instruction = CPU::Branch<Conditional::OverflowClear>{0x10};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'BVC $10'") { REQUIRE(disassembly == "BVC $10"); }
    }

    WHEN("We disassemble a BVS instruction") {
      auto instruction = CPU::Branch<Conditional::OverflowSet>{0x10};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'BVS $10'") { REQUIRE(disassembly == "BVS $10"); }
    }

    WHEN("We disassemble a JMP absolute instruction") {
      auto instruction = CPU::Jump<AddressingMode::Absolute>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'JMP $1234'") { REQUIRE(disassembly == "JMP $1234"); }
    }

    WHEN("We disassemble a JMP indirect instruction") {
      auto instruction = CPU::Jump<AddressingMode::Indirect>{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'JMP ($1234)'") { REQUIRE(disassembly == "JMP ($1234)"); }
    }

    WHEN("We disassemble a JSR instruction") {
      auto instruction = CPU::JumpToSubroutine{0x1234};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'JSR $1234'") { REQUIRE(disassembly == "JSR $1234"); }
    }

    WHEN("We disassemble a RTS instruction") {
      auto instruction = CPU::ReturnFromSubroutine{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'RTS'") { REQUIRE(disassembly == "RTS"); }
    }

    WHEN("We disassemble a RTI instruction") {
      auto instruction = CPU::ReturnFromInterrupt{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'RTI'") { REQUIRE(disassembly == "RTI"); }
    }

    WHEN("We disassemble a CLC instruction") {
      auto instruction = CPU::ClearStatusFlag<CPU::StatusFlag::Carry>{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'CLC'") { REQUIRE(disassembly == "CLC"); }
    }

    WHEN("We disassemble a CLD instruction") {
      auto instruction = CPU::ClearStatusFlag<CPU::StatusFlag::DecimalMode>{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'CLD'") { REQUIRE(disassembly == "CLD"); }
    }

    WHEN("We disassemble a CLI instruction") {
      auto instruction = CPU::ClearStatusFlag<CPU::StatusFlag::InterruptDisable>{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'CLI'") { REQUIRE(disassembly == "CLI"); }
    }

    WHEN("We disassemble a CLV instruction") {
      auto instruction = CPU::ClearStatusFlag<CPU::StatusFlag::Overflow>{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'CLV'") { REQUIRE(disassembly == "CLV"); }
    }

    WHEN("We disassemble a SEC instruction") {
      auto instruction = CPU::SetStatusFlag<CPU::StatusFlag::Carry>{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'SEC'") { REQUIRE(disassembly == "SEC"); }
    }

    WHEN("We disassemble a SED instruction") {
      auto instruction = CPU::SetStatusFlag<CPU::StatusFlag::DecimalMode>{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'SED'") { REQUIRE(disassembly == "SED"); }
    }

    WHEN("We disassemble a SEI instruction") {
      auto instruction = CPU::SetStatusFlag<CPU::StatusFlag::InterruptDisable>{};
      auto disassembly = cpu.DisassembleInstruction(instruction);

      THEN("It should disassemble to 'SEI'") { REQUIRE(disassembly == "SEI"); }
    }
  }
}
