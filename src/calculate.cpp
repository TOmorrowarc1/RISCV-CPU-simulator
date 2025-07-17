#include "calculate.hpp"
#include <cassert>

ALU &ALU::getInstance() {
  static ALU instance;
  return instance;
}

unsigned int ALU::calculate(Control::EXEControlInfo::CalcType calc_type,
                            unsigned int input1, unsigned int input2) {
  unsigned int result = 0;
  switch (calc_type) {
  case Control::EXEControlInfo::CalcType::ADD:
    result = input1 + input2;
    break;
  case Control::EXEControlInfo::CalcType::SUB:
    result = input1 - input2;
    break;
  case Control::EXEControlInfo::CalcType::SLL:
    result = input1 << input2;
    break;
  case Control::EXEControlInfo::CalcType::LT:
    // static_cast for 3 times.
    result = int(input1) < int(input2);
    break;
  case Control::EXEControlInfo::CalcType::LTU:
    result = input1 < input2;
    break;
  case Control::EXEControlInfo::CalcType::GE:
    result = int(input1) >= int(input2);
    break;
  case Control::EXEControlInfo::CalcType::GEU:
    result = input1 >= input2;
    break;
  case Control::EXEControlInfo::CalcType::EQ:
    result = input1 == input2;
    break;
  case Control::EXEControlInfo::CalcType::NE:
    result = input1 != input2;
    break;
  case Control::EXEControlInfo::CalcType::SRL:
    result = input1 >> input2;
    break;
  case Control::EXEControlInfo::CalcType::SRA:
    result = int(input1) >> input2;
    break;
  case Control::EXEControlInfo::CalcType::OR:
    result = input1 | input2;
    break;
  case Control::EXEControlInfo::CalcType::AND:
    result = input1 & input2;
    break;
  case Control::EXEControlInfo::CalcType::XOR:
    result = input1 ^ input2;
    break;
  case Control::EXEControlInfo::CalcType::PRINTIMM:
    result = input2;
    break;
  default:
    throw std::exception();
  }
  return result;
}