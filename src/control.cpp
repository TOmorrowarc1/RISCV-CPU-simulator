#include "control.hpp"
#include <cassert>

Instruction::Instruction() { this->command_ = 0; }
Instruction::Instruction(unsigned int command) { this->command_ = command; }
void Instruction::parse() {
  operand_ = command_ & 0x7f;
  switch (operand_) {
  case 0x33: {
    rd_ = (command_ >> 7) & 0x1f;
    register1_ = (command_ >> 15) & 0x1f;
    register2_ = (command_ >> 20) & 0x1f;
    func3_ = (command_ >> 12) & 0x7;
    func7_ = (command_ >> 25) & 0x7f;
    switch (func3_) {
    case 0x0:
      if (func7_ == 0x00) {
        instruction_type_ = Type::ADD;
      } else if (func7_ == 0x20) {
        instruction_type_ = Type::SUB;
      } else {
        assert(false);
      }
      break;
    case 0x1:
      instruction_type_ = Type::SLL;
      break;
    case 0x2:
      instruction_type_ = Type::SLT;
      break;
    case 0x3:
      instruction_type_ = Type::SLTU;
      break;
    case 0x4:
      instruction_type_ = Type::XOR;
      break;
    case 0x5:
      if (func7_ == 0x00) {
        instruction_type_ = Type::SRL;
      } else if (func7_ == 0x20) {
        instruction_type_ = Type::SRA;
      } else {
        assert(false);
      }
      break;
    case 0x6:
      instruction_type_ = Type::OR;
      break;
    case 0x7:
      instruction_type_ = Type::AND;
      break;
    default:
      assert(false);
    }
    break;
  }
  case 0x13: {
    rd_ = (command_ >> 7) & 0x1f;
    register1_ = (command_ >> 15) & 0x1f;
    func3_ = (command_ >> 12) & 0x7;
    immediate_ = int(command_) >> 20;
    switch (func3_) {
    case 0x0:
      instruction_type_ = Type::ADDI;
      break;
    case 0x1:
      instruction_type_ = Type::SLLI;
      break;
    case 0x2:
      instruction_type_ = Type::SLTI;
      break;
    case 0x3:
      instruction_type_ = Type::SLTIU;
      break;
    case 0x4:
      instruction_type_ = Type::XORI;
      break;
    case 0x5:
      func7_ = (command_ >> 27) & 0x1f;
      immediate_ = immediate_ & 0x1f;
      if (func7_ == 0x0) {
        instruction_type_ = Type::SRLI;
      } else if (func7_ == 0x8) {
        instruction_type_ = Type::SRAI;
      } else {
        assert(false);
      }
      break;
    case 0x6:
      instruction_type_ = Type::ORI;
      break;
    case 0x7:
      instruction_type_ = Type::ANDI;
      break;
    default:
      assert(false);
    }
    break;
  }
  case 0x3: {
    rd_ = (command_ >> 7) & 0x1f;
    register1_ = (command_ >> 15) & 0x1f;
    func3_ = (command_ >> 12) & 0x7;
    immediate_ = int(command_) >> 20;
    switch (func3_) {
    case 0x0:
      instruction_type_ = Type::LB;
      break;
    case 0x1:
      instruction_type_ = Type::LH;
      break;
    case 0x2:
      instruction_type_ = Type::LW;
      break;
    case 0x4:
      instruction_type_ = Type::LBU;
      break;
    case 0x5:
      instruction_type_ = Type::LHU;
      break;
    default:
      assert(false);
    }
    break;
  }
  case 0x67: {
    rd_ = (command_ >> 7) & 0x1f;
    register1_ = (command_ >> 15) & 0x1f;
    func3_ = (command_ >> 12) & 0x7;
    immediate_ = int(command_) >> 20;
    if (func3_ == 0x0) {
      instruction_type_ = Type::JALR;
    } else {
      assert(false);
    }
    break;
  }
  case 0x23: {
    func3_ = (command_ >> 12) & 0x7;
    register1_ = (command_ >> 15) & 0x1f;
    register2_ = (command_ >> 20) & 0x1f;
    immediate_ = int(((((command_ >> 25) & 0x7f) << 5) | ((command_ >> 7) & 0x1f))
                    << 20) >>
                20;
    switch (func3_) {
    case 0x0:
      instruction_type_ = Type::SB;
      break;
    case 0x1:
      instruction_type_ = Type::SH;
      break;
    case 0x2:
      instruction_type_ = Type::SW;
      break;
    default:
      assert(false);
    }
    break;
  }
  case 0x63: {
    register1_ = (command_ >> 15) & 0x1f;
    register2_ = (command_ >> 20) & 0x1f;
    func3_ = (command_ >> 12) & 0x7;
    immediate_ =
        int(((((command_ >> 7) & 0x1) << 11) | (((command_ >> 8) & 0xf) << 1) |
             (((command_ >> 25) & 0x3f) << 5) | (((command_ >> 31) & 0x1) << 12))
            << 19) >>
        19;
    switch (func3_) {
    case 0x0:
      instruction_type_ = Type::BEQ;
      break;
    case 0x1:
      instruction_type_ = Type::BNE;
      break;
    case 0x4:
      instruction_type_ = Type::BLT;
      break;
    case 0x5:
      instruction_type_ = Type::BGE;
      break;
    case 0x6:
      instruction_type_ = Type::BLTU;
      break;
    case 0x7:
      instruction_type_ = Type::BGEU;
      break;
    default:
      assert(false);
    }
    break;
  }
  case 0x37: {
    rd_ = (command_ >> 7) & 0x1f;
    immediate_ = command_ & 0xfffff000;
    instruction_type_ = Type::LUI;
    break;
  }
  case 0x17: {
    rd_ = (command_ >> 7) & 0x1f;
    immediate_ = command_ & 0xfffff000;
    instruction_type_ = Type::AUIPC;
    break;
  }
  case 0x6f: {
    rd_ = (command_ >> 7) & 0x1f;
    immediate_ =
        int(((((command_ >> 12) & 0xff) << 12) |
             (((command_ >> 20) & 0x1) << 11) |
             (((command_ >> 21) & 0x3ff) << 1) | (((command_ >> 31) & 1) << 20))
            << 11) >>
        11;
    instruction_type_ = Type::JAL;
    break;
  }
  default: {
    assert(0);
  }
  }
}

InsBoard::InsBoard() {
  for (int i = 0; i < 3; ++i) {
    instructions_[i] = 0;
  }
}

InsBoard &InsBoard::getInstance() {
  static InsBoard instance;
  return instance;
}

void InsBoard::insLaunch(Instruction &target) {
  target.parse();
  std::unique_lock<std::shared_mutex>(RWlock);
  instructions_[0] = target;
}

Instruction InsBoard::readExecute() {
  std::shared_lock<std::shared_mutex>(RWlock);
  return instructions_[1];
}

Instruction InsBoard::readMEM() {
  std::shared_lock<std::shared_mutex>(RWlock);
  return instructions_[2];
}

Instruction InsBoard::readWB() {
  std::shared_lock<std::shared_mutex>(RWlock);
  return instructions_[3];
}

void InsBoard::refreshStage() {
  std::unique_lock<std::shared_mutex>(RWlock);
  for (int i = 3; i > 0; --i) {
    instructions_[i] = instructions_[i - 1];
  }
}