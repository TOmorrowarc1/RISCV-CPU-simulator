#include "control.hpp"
#include <cassert>

Instruction::Instruction() { this->command = 0; }
Instruction::Instruction(unsigned int command) { this->command = command; }
void Instruction::parse() {
  operand = command & 0x7f;
  switch (operand) {
  case 0x33: {
    rd = (command >> 7) & 0x1f;
    register1 = (command >> 15) & 0x1f;
    register2 = (command >> 20) & 0x1f;
    func3 = (command >> 12) & 0x7;
    func7 = (command >> 25) & 0x7f;
    switch (func3) {
    case 0x0:
      if (func7 == 0x00) {
        instructionType = Type::ADD;
      } else if (func7 == 0x20) {
        instructionType = Type::SUB;
      } else {
        assert(false);
      }
      break;
    case 0x1:
      instructionType = Type::SLL;
      break;
    case 0x2:
      instructionType = Type::SLT;
      break;
    case 0x3:
      instructionType = Type::SLTU;
      break;
    case 0x4:
      instructionType = Type::XOR;
      break;
    case 0x5:
      if (func7 == 0x00) {
        instructionType = Type::SRL;
      } else if (func7 == 0x20) {
        instructionType = Type::SRA;
      } else {
        assert(false);
      }
      break;
    case 0x6:
      instructionType = Type::OR;
      break;
    case 0x7:
      instructionType = Type::AND;
      break;
    default:
      assert(false);
    }
    break;
  }
  case 0x13: {
    rd = (command >> 7) & 0x1f;
    register1 = (command >> 15) & 0x1f;
    func3 = (command >> 12) & 0x7;
    immediate = int(command) >> 20;
    switch (func3) {
    case 0x0:
      instructionType = Type::ADDI;
      break;
    case 0x1:
      instructionType = Type::SLLI;
      break;
    case 0x2:
      instructionType = Type::SLTI;
      break;
    case 0x3:
      instructionType = Type::SLTIU;
      break;
    case 0x4:
      instructionType = Type::XORI;
      break;
    case 0x5:
      func7 = (command >> 27) & 0x1f;
      immediate = immediate & 0x1f;
      if (func7 == 0x0) {
        instructionType = Type::SRLI;
      } else if (func7 == 0x8) {
        instructionType = Type::SRAI;
      } else {
        assert(false);
      }
      break;
    case 0x6:
      instructionType = Type::ORI;
      break;
    case 0x7:
      instructionType = Type::ANDI;
      break;
    default:
      assert(false);
    }
    break;
  }
  case 0x3: {
    rd = (command >> 7) & 0x1f;
    register1 = (command >> 15) & 0x1f;
    func3 = (command >> 12) & 0x7;
    immediate = int(command) >> 20;
    switch (func3) {
    case 0x0:
      instructionType = Type::LB;
      break;
    case 0x1:
      instructionType = Type::LH;
      break;
    case 0x2:
      instructionType = Type::LW;
      break;
    case 0x4:
      instructionType = Type::LBU;
      break;
    case 0x5:
      instructionType = Type::LHU;
      break;
    default:
      assert(false);
    }
    break;
  }
  case 0x67: {
    rd = (command >> 7) & 0x1f;
    register1 = (command >> 15) & 0x1f;
    func3 = (command >> 12) & 0x7;
    immediate = int(command) >> 20;
    if (func3 == 0x0) {
      instructionType = Type::JALR;
    } else {
      assert(false);
    }
    break;
  }
  case 0x23: {
    func3 = (command >> 12) & 0x7;
    register1 = (command >> 15) & 0x1f;
    register2 = (command >> 20) & 0x1f;
    immediate = int(((((command >> 25) & 0x7f) << 5) | ((command >> 7) & 0x1f))
                    << 20) >>
                20;
    switch (func3) {
    case 0x0:
      instructionType = Type::SB;
      break;
    case 0x1:
      instructionType = Type::SH;
      break;
    case 0x2:
      instructionType = Type::SW;
      break;
    default:
      assert(false);
    }
    break;
  }
  case 0x63: {
    register1 = (command >> 15) & 0x1f;
    register2 = (command >> 20) & 0x1f;
    func3 = (command >> 12) & 0x7;
    immediate =
        int(((((command >> 7) & 0x1) << 11) | (((command >> 8) & 0xf) << 1) |
             (((command >> 25) & 0x3f) << 5) | (((command >> 31) & 0x1) << 12))
            << 19) >>
        19;
    switch (func3) {
    case 0x0:
      instructionType = Type::BEQ;
      break;
    case 0x1:
      instructionType = Type::BNE;
      break;
    case 0x4:
      instructionType = Type::BLT;
      break;
    case 0x5:
      instructionType = Type::BGE;
      break;
    case 0x6:
      instructionType = Type::BLTU;
      break;
    case 0x7:
      instructionType = Type::BGEU;
      break;
    default:
      assert(false);
    }
    break;
  }
  case 0x37: {
    rd = (command >> 7) & 0x1f;
    immediate = command & 0xfffff000;
    instructionType = Type::LUI;
    break;
  }
  case 0x17: {
    rd = (command >> 7) & 0x1f;
    immediate = command & 0xfffff000;
    instructionType = Type::AUIPC;
    break;
  }
  case 0x6f: {
    rd = (command >> 7) & 0x1f;
    immediate =
        int(((((command >> 12) & 0xff) << 12) |
             (((command >> 20) & 0x1) << 11) |
             (((command >> 21) & 0x3ff) << 1) | (((command >> 31) & 1) << 20))
            << 11) >>
        11;
    instructionType = Type::JAL;
    break;
  }
  default: {
    assert(0);
  }
  }
}

InsBoard::InsBoard() {
  for (int i = 0; i < 3; ++i) {
    Instructions[i] = 0;
  }
}

InsBoard &InsBoard::getInstance() {
  static InsBoard instance;
  return instance;
}

void InsBoard::insLaunch(Instruction &target) {
  target.parse();
  std::unique_lock<std::shared_mutex>(RWlock);
  Instructions[0] = target;
}

Instruction InsBoard::readExecute() {
  std::shared_lock<std::shared_mutex>(RWlock);
  return Instructions[1];
}

Instruction InsBoard::readMEM() {
  std::shared_lock<std::shared_mutex>(RWlock);
  return Instructions[2];
}

Instruction InsBoard::readWB() {
  std::shared_lock<std::shared_mutex>(RWlock);
  return Instructions[3];
}

void InsBoard::refreshStage() {
  std::unique_lock<std::shared_mutex>(RWlock);
  for (int i = 3; i > 0; --i) {
    Instructions[i] = Instructions[i - 1];
  }
}