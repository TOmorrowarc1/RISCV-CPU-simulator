#include "control.hpp"
#include <cassert>

Instruction::Instruction() { command_ = 0; }
Instruction::Instruction(unsigned int command) { command_ = command; }

Control::AllControlInfo Instruction::parse() {
  Control::AllControlInfo result;
  result.exe_control.pc = pc_next_;
  unsigned int operand_ = command_ & 0x7f;
  switch (operand_) {
  case 0x33: {
    // For type 'R':
    result.reg_control.register1 = (command_ >> 15) & 0x1f;
    result.reg_control.register2 = (command_ >> 20) & 0x1f;
    result.exe_control.signImmediate = false;
    result.wb_control.allow = true;
    result.wb_control.rd = (command_ >> 7) & 0x1f;
    unsigned int func3_ = (command_ >> 12) & 0x7;
    unsigned int func7_ = (command_ >> 25) & 0x7f;
    switch (func3_) {
    case 0x0:
      if (func7_ == 0x00) {
        result.exe_control.type = Control::EXEControlInfo::CalcType::ADD;
      } else if (func7_ == 0x20) {
        result.exe_control.type = Control::EXEControlInfo::CalcType::SUB;
      } else {
        assert(false);
      }
      break;
    case 0x1:
      result.exe_control.type = Control::EXEControlInfo::CalcType::SLL;
      break;
    case 0x2:
      result.exe_control.type = Control::EXEControlInfo::CalcType::LT;
      break;
    case 0x3:
      result.exe_control.type = Control::EXEControlInfo::CalcType::LTU;
      break;
    case 0x4:
      result.exe_control.type = Control::EXEControlInfo::CalcType::XOR;
      break;
    case 0x5:
      if (func7_ == 0x00) {
        result.exe_control.type = Control::EXEControlInfo::CalcType::SRL;
      } else if (func7_ == 0x20) {
        result.exe_control.type = Control::EXEControlInfo::CalcType::SRA;
      } else {
        assert(false);
      }
      break;
    case 0x6:
      result.exe_control.type = Control::EXEControlInfo::CalcType::OR;
      break;
    case 0x7:
      result.exe_control.type = Control::EXEControlInfo::CalcType::AND;
      break;
    default:
      assert(false);
    }
    break;
  }
  case 0x13: {
    result.reg_control.register1 = (command_ >> 15) & 0x1f;
    result.exe_control.signImmediate = true;
    result.exe_control.immdiate = int(command_) >> 20;
    result.wb_control.allow = true;
    result.wb_control.rd = (command_ >> 7) & 0x1f;
    unsigned int func3_ = (command_ >> 12) & 0x7;
    unsigned int func7_ = (command_ >> 25) & 0x7f;
    switch (func3_) {
    case 0x0:
      result.exe_control.type = Control::EXEControlInfo::CalcType::ADD;
      break;
    case 0x1:
      result.exe_control.type = Control::EXEControlInfo::CalcType::SLL;
      break;
    case 0x2:
      result.exe_control.type = Control::EXEControlInfo::CalcType::LT;
      break;
    case 0x3:
      result.exe_control.type = Control::EXEControlInfo::CalcType::LTU;
      break;
    case 0x4:
      result.exe_control.type = Control::EXEControlInfo::CalcType::XOR;
      break;
    case 0x5:
      func7_ = (command_ >> 27) & 0x1f;
      result.exe_control.immdiate = result.exe_control.immdiate & 0x1f;
      if (func7_ == 0x0) {
        result.exe_control.type = Control::EXEControlInfo::CalcType::SRL;
      } else if (func7_ == 0x8) {
        result.exe_control.type = Control::EXEControlInfo::CalcType::SRA;
      } else {
        assert(false);
      }
      break;
    case 0x6:
      result.exe_control.type = Control::EXEControlInfo::CalcType::OR;
      break;
    case 0x7:
      result.exe_control.type = Control::EXEControlInfo::CalcType::AND;
      break;
    default:
      assert(false);
    }
    break;
  }
  case 0x3: {
    result.reg_control.register1 = (command_ >> 15) & 0x1f;
    result.exe_control.signImmediate = true;
    result.exe_control.immdiate = int(command_) >> 20;
    result.mem_control.type = Control::MEMControlInfo::MemType::LOAD;
    result.wb_control.allow = true;
    result.wb_control.rd = (command_ >> 7) & 0x1f;
    unsigned int func3_ = (command_ >> 12) & 0x7;
    switch (func3_) {
    case 0x0:
      result.mem_control.size = Control::MEMControlInfo::MemSize::BYTE;
      result.mem_control.extend_sign = true;
      break;
    case 0x1:
      result.mem_control.size = Control::MEMControlInfo::MemSize::HALF;
      result.mem_control.extend_sign = true;
      break;
    case 0x2:
      result.mem_control.size = Control::MEMControlInfo::MemSize::WORD;
      result.mem_control.extend_sign = true;
      break;
    case 0x4:
      result.mem_control.size = Control::MEMControlInfo::MemSize::BYTE;
      result.mem_control.extend_sign = false;
      break;
    case 0x5:
      result.mem_control.size = Control::MEMControlInfo::MemSize::HALF;
      result.mem_control.extend_sign = false;
      break;
    default:
      assert(false);
    }
    break;
  }
  case 0x67: {
    result.reg_control.register1 = (command_ >> 15) & 0x1f;
    result.exe_control.signImmediate = true;
    result.exe_control.immdiate = int(command_) >> 20;
    result.wb_control.allow = true;
    result.wb_control.rd = (command_ >> 7) & 0x1f;
    unsigned int func3_ = (command_ >> 12) & 0x7;
    if (func3_ == 0x0) {
      result.exe_control.jump = Control::EXEControlInfo::JumpType::JALR;
    } else {
      assert(false);
    }
    break;
  }
  case 0x23: {
    result.reg_control.register1 = (command_ >> 15) & 0x1f;
    result.reg_control.register2 = (command_ >> 20) & 0x1f;
    result.exe_control.immdiate =
        int(((((command_ >> 25) & 0x7f) << 5) | ((command_ >> 7) & 0x1f))
            << 20) >>
        20;
    result.mem_control.type = Control::MEMControlInfo::MemType::STORE;
    unsigned int func3_ = (command_ >> 12) & 0x7;
    switch (func3_) {
    case 0x0:
      result.mem_control.size = Control::MEMControlInfo::MemSize::BYTE;
      break;
    case 0x1:
      result.mem_control.size = Control::MEMControlInfo::MemSize::HALF;
      break;
    case 0x2:
      result.mem_control.size = Control::MEMControlInfo::MemSize::WORD;
      break;
    default:
      assert(false);
    }
    break;
  }
  case 0x63: {
    result.reg_control.register1 = (command_ >> 15) & 0x1f;
    result.reg_control.register2 = (command_ >> 20) & 0x1f;
    result.exe_control.jump = has_jump_
                                  ? Control::EXEControlInfo::JumpType::BYES
                                  : Control::EXEControlInfo::JumpType::BNO;
    result.exe_control.immdiate =
        int(((((command_ >> 7) & 0x1) << 11) | (((command_ >> 8) & 0xf) << 1) |
             (((command_ >> 25) & 0x3f) << 5) |
             (((command_ >> 31) & 0x1) << 12))
            << 19) >>
        19;
    unsigned int func3_ = (command_ >> 12) & 0x7;
    switch (func3_) {
    case 0x0:
      result.exe_control.type = Control::EXEControlInfo::CalcType::EQ;
      break;
    case 0x1:
      result.exe_control.type = Control::EXEControlInfo::CalcType::NE;
      break;
    case 0x4:
      result.exe_control.type = Control::EXEControlInfo::CalcType::LT;
      break;
    case 0x5:
      result.exe_control.type = Control::EXEControlInfo::CalcType::GE;
      break;
    case 0x6:
      result.exe_control.type = Control::EXEControlInfo::CalcType::LTU;
      break;
    case 0x7:
      result.exe_control.type = Control::EXEControlInfo::CalcType::GEU;
      break;
    default:
      assert(false);
    }
    break;
  }
  case 0x37: {
    result.exe_control.type = Control::EXEControlInfo::CalcType::PRINTIMM;
    result.exe_control.signImmediate = true;
    result.exe_control.immdiate = int(command_ & 0xfffff000) << 12;
    result.wb_control.allow = true;
    result.wb_control.rd = (command_ >> 7) & 0x1f;
    break;
  }
  case 0x17: {
    result.exe_control.type = Control::EXEControlInfo::CalcType::ADD;
    result.exe_control.signImmediate = true;
    result.exe_control.immdiate = int(command_ & 0xfffff000) << 12;
    result.wb_control.allow = true;
    result.wb_control.rd = (command_ >> 7) & 0x1f;
    break;
  }
  case 0x6f: {
    /*
    type_ = 'J';
    rd_ = (command_ >> 7) & 0x1f;
    immediate_ =
        int(((((command_ >> 12) & 0xff) << 12) |
             (((command_ >> 20) & 0x1) << 11) |
             (((command_ >> 21) & 0x3ff) << 1) | (((command_ >> 31) & 1) << 20))
            << 11) >>
        11;
    name_ = Name::JAL;
    break;*/
  }
  default: {
    assert(false);
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
