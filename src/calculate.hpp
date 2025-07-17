#ifndef CALCULATE_HPP
#define CALCULATE_HPP
#include "control.hpp"
class ALU {
private:
  ALU();

public:
  static ALU &getInstance();
  int chooseSecondInput(bool sign1, bool sign2, int reg2, int imm, int forward);
  int calculate(Instruction::Name calc_type, int input1, int input2);
};
#endif