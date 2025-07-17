#include "calculate.hpp"
#include <cassert>

ALU &ALU::getInstance() {
  static ALU instance;
  return instance;
}

int ALU::chooseSecondInput(bool sign1, bool sign2, int reg2, int imm,
                           int forward) {
  int result = sign1 ? reg2 : imm;
  result = sign2 ? forward : result;
  return result;
}

int ALU::calculate(Instruction::Name calc_type, int input1, int input2) {
  int result = 0;
  switch (calc_type) {
    case 
  }
  return std::pair<bool, int>;
}