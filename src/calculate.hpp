#ifndef CALCULATE_HPP
#define CALCULATE_HPP
#include "control.hpp"
class ALU {
private:
  ALU();

public:
  static ALU &getInstance();
  unsigned int calculate(Control::EXEControlInfo::CalcType calc_type,
                         unsigned int operand1, unsigned int operand2);
};
#endif