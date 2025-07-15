#include "control.hpp"
#include <cassert>

Instruction::Instruction() { this->command = 0; }
Instruction::Instruction(unsigned int command) { this->command = command; }
void Instruction::parse() {
  operand = command & 0x7f;
  switch (operand) {
  case 0x33: {
  }
  case 0x13: {
  }
  case 0x3: {
  }
  case 0x67: {
  }
  case 0x23: {
  }
  case 0x63: {
  }
  case 0x37: {
  }
  case 0x17: {
  }
  case 0x6f: {
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