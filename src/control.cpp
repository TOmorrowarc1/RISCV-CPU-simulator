#include "control.hpp"
#include <cassert>

Instruction::Instruction() { this->command_ = 0; }
Instruction::Instruction(unsigned int command) { this->command_ = command; }

std::pair<char, Instruction::Name> Instruction::getType() {
  return std::pair<char, Instruction::Name>(type_, name_);
}
std::pair<unsigned int, unsigned int> Instruction::getRegs() {
  return std::pair<unsigned int, unsigned int>(register1_, register2_);
}
unsigned int Instruction::getRd() { return rd_; }
int Instruction::getImmdiate() { return immediate_; }
void Instruction::parse() {
  operand_ = command_ & 0x7f;
  switch (operand_) {
  case 0x33: {
    type_ = 'R';
    rd_ = (command_ >> 7) & 0x1f;
    register1_ = (command_ >> 15) & 0x1f;
    register2_ = (command_ >> 20) & 0x1f;
    func3_ = (command_ >> 12) & 0x7;
    func7_ = (command_ >> 25) & 0x7f;
    switch (func3_) {
    case 0x0:
      if (func7_ == 0x00) {
        name_ = Name::ADD;
      } else if (func7_ == 0x20) {
        name_ = Name::SUB;
      } else {
        assert(false);
      }
      break;
    case 0x1:
      name_ = Name::SLL;
      break;
    case 0x2:
      name_ = Name::SLT;
      break;
    case 0x3:
      name_ = Name::SLTU;
      break;
    case 0x4:
      name_ = Name::XOR;
      break;
    case 0x5:
      if (func7_ == 0x00) {
        name_ = Name::SRL;
      } else if (func7_ == 0x20) {
        name_ = Name::SRA;
      } else {
        assert(false);
      }
      break;
    case 0x6:
      name_ = Name::OR;
      break;
    case 0x7:
      name_ = Name::AND;
      break;
    default:
      assert(false);
    }
    break;
  }
  case 0x13: {
    type_ = 'I';
    rd_ = (command_ >> 7) & 0x1f;
    register1_ = (command_ >> 15) & 0x1f;
    func3_ = (command_ >> 12) & 0x7;
    immediate_ = int(command_) >> 20;
    switch (func3_) {
    case 0x0:
      name_ = Name::ADDI;
      break;
    case 0x1:
      name_ = Name::SLLI;
      break;
    case 0x2:
      name_ = Name::SLTI;
      break;
    case 0x3:
      name_ = Name::SLTIU;
      break;
    case 0x4:
      name_ = Name::XORI;
      break;
    case 0x5:
      func7_ = (command_ >> 27) & 0x1f;
      immediate_ = immediate_ & 0x1f;
      if (func7_ == 0x0) {
        name_ = Name::SRLI;
      } else if (func7_ == 0x8) {
        name_ = Name::SRAI;
      } else {
        assert(false);
      }
      break;
    case 0x6:
      name_ = Name::ORI;
      break;
    case 0x7:
      name_ = Name::ANDI;
      break;
    default:
      assert(false);
    }
    break;
  }
  case 0x3: {
    type_ = 'I';
    rd_ = (command_ >> 7) & 0x1f;
    register1_ = (command_ >> 15) & 0x1f;
    func3_ = (command_ >> 12) & 0x7;
    immediate_ = int(command_) >> 20;
    switch (func3_) {
    case 0x0:
      name_ = Name::LB;
      break;
    case 0x1:
      name_ = Name::LH;
      break;
    case 0x2:
      name_ = Name::LW;
      break;
    case 0x4:
      name_ = Name::LBU;
      break;
    case 0x5:
      name_ = Name::LHU;
      break;
    default:
      assert(false);
    }
    break;
  }
  case 0x67: {
    type_ = 'I';
    rd_ = (command_ >> 7) & 0x1f;
    register1_ = (command_ >> 15) & 0x1f;
    func3_ = (command_ >> 12) & 0x7;
    immediate_ = int(command_) >> 20;
    if (func3_ == 0x0) {
      name_ = Name::JALR;
    } else {
      assert(false);
    }
    break;
  }
  case 0x23: {
    type_ = 'S';
    func3_ = (command_ >> 12) & 0x7;
    register1_ = (command_ >> 15) & 0x1f;
    register2_ = (command_ >> 20) & 0x1f;
    immediate_ =
        int(((((command_ >> 25) & 0x7f) << 5) | ((command_ >> 7) & 0x1f))
            << 20) >>
        20;
    switch (func3_) {
    case 0x0:
      name_ = Name::SB;
      break;
    case 0x1:
      name_ = Name::SH;
      break;
    case 0x2:
      name_ = Name::SW;
      break;
    default:
      assert(false);
    }
    break;
  }
  case 0x63: {
    type_ = 'B';
    register1_ = (command_ >> 15) & 0x1f;
    register2_ = (command_ >> 20) & 0x1f;
    func3_ = (command_ >> 12) & 0x7;
    immediate_ =
        int(((((command_ >> 7) & 0x1) << 11) | (((command_ >> 8) & 0xf) << 1) |
             (((command_ >> 25) & 0x3f) << 5) |
             (((command_ >> 31) & 0x1) << 12))
            << 19) >>
        19;
    switch (func3_) {
    case 0x0:
      name_ = Name::BEQ;
      break;
    case 0x1:
      name_ = Name::BNE;
      break;
    case 0x4:
      name_ = Name::BLT;
      break;
    case 0x5:
      name_ = Name::BGE;
      break;
    case 0x6:
      name_ = Name::BLTU;
      break;
    case 0x7:
      name_ = Name::BGEU;
      break;
    default:
      assert(false);
    }
    break;
  }
  case 0x37: {
    type_ = 'U';
    rd_ = (command_ >> 7) & 0x1f;
    immediate_ = int(command_ & 0xfffff000);
    name_ = Name::LUI;
    break;
  }
  case 0x17: {
    type_ = 'U';
    rd_ = (command_ >> 7) & 0x1f;
    immediate_ = int(command_ & 0xfffff000);
    name_ = Name::AUIPC;
    break;
  }
  case 0x6f: {
    type_ = 'J';
    rd_ = (command_ >> 7) & 0x1f;
    immediate_ =
        int(((((command_ >> 12) & 0xff) << 12) |
             (((command_ >> 20) & 0x1) << 11) |
             (((command_ >> 21) & 0x3ff) << 1) | (((command_ >> 31) & 1) << 20))
            << 11) >>
        11;
    name_ = Name::JAL;
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