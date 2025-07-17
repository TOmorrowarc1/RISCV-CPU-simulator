#include "control.hpp"
#include "storage.hpp"
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
    // for type 'I': immediate calculation.
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
    // for type 'I': load reg2 into reg1+imm.
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
    // for type 'I': JALR- pc=reg1+imm, rd=pc+4
    result.reg_control.register1 = (command_ >> 15) & 0x1f;
    result.exe_control.signResultPC = true;
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
    // for type 'S': store reg2 into reg1+imm;
    result.reg_control.register1 = (command_ >> 15) & 0x1f;
    result.reg_control.register2 = (command_ >> 20) & 0x1f;
    result.exe_control.signImmediate = true;
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
    // for type 'B': conditional branch: examine the prediction and write back
    // the possible result, which stores in the "pc" of ins.
    result.reg_control.register1 = (command_ >> 15) & 0x1f;
    result.reg_control.register2 = (command_ >> 20) & 0x1f;
    result.exe_control.jump = has_jump_
                                  ? Control::EXEControlInfo::JumpType::BYES
                                  : Control::EXEControlInfo::JumpType::BNO;
    /*result.exe_control.immdiate =
        int(((((command_ >> 7) & 0x1) << 11) | (((command_ >> 8) & 0xf) << 1) |
             (((command_ >> 25) & 0x3f) << 5) |
             (((command_ >> 31) & 0x1) << 12))
            << 19) >>
        19;*/
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
    // for type 'U': LUI ins-ALU do nothing.
    result.exe_control.type = Control::EXEControlInfo::CalcType::PRINTIMM;
    result.exe_control.signImmediate = true;
    result.exe_control.immdiate = int(command_ & 0xfffff000) << 12;
    result.wb_control.allow = true;
    result.wb_control.rd = (command_ >> 7) & 0x1f;
    break;
  }
  case 0x17: {
    // for type 'U': AUIPC ins-PC+imm, what a black sheep!
    result.exe_control.type = Control::EXEControlInfo::CalcType::ADD;
    result.exe_control.signPC = true;
    result.exe_control.signImmediate = true;
    result.exe_control.immdiate = int(command_ & 0xfffff000) << 12;
    result.wb_control.allow = true;
    result.wb_control.rd = (command_ >> 7) & 0x1f;
    break;
  }
  case 0x6f: {
    // for type 'J': JAL: just write the rd.
    result.exe_control.signResultPC = true;
    result.wb_control.allow = true;
    result.wb_control.rd = (command_ >> 7) & 0x1f;
    /*immediate_ =
        int(((((command_ >> 12) & 0xff) << 12) |
             (((command_ >> 20) & 0x1) << 11) |
             (((command_ >> 21) & 0x3ff) << 1) | (((command_ >> 31) & 1) << 20))
            << 11) >>
        11;*/
  }
  default: {
    assert(false);
  }
  }
}

InsBoard::InsBoard() {
  std::unique_lock<std::shared_mutex>(rwlock_);
  for (int i = 0; i < 4; ++i) {
    exe_control_.push_front(Control::EXEControlInfo());
    mem_control_.push_front(Control::MEMControlInfo());
    wb_control_.push_front(Control::WBControlInfo());
  }
}

InsBoard &InsBoard::getInstance() {
  static InsBoard instance;
  return instance;
}

Control::RegControlInfo InsBoard::IR(Instruction &target) {
  /*
  First interpret the instruction, then compare the result with signals in EXE
  and LS for forwarding. After all signals prepared well, launch them onto
  board.
  */
  Control::AllControlInfo control_signals = target.parse();
  int register1 = control_signals.reg_control.register1;
  int register2 = control_signals.reg_control.register2;
  Control::WBControlInfo exe_write_back;
  Control::WBControlInfo mem_write_back;
  Control::MEMControlInfo exe_mem_control;
  {
    std::shared_lock<std::shared_mutex>(rwlock_);
    exe_write_back = wb_control_[1];
    mem_write_back = wb_control_[2];
    exe_mem_control = mem_control_[1];
  }

  if (mem_write_back.allow) {
    if (mem_write_back.rd == register1) {
      control_signals.exe_control.signForward1 = 2;
    } else if (mem_write_back.rd == register2) {
      control_signals.exe_control.signForward2 = 2;
    }
  }
  if (exe_write_back.allow) {
    bool flag = false;
    if (exe_write_back.rd == register1) {
      control_signals.exe_control.signForward1 = 1;
      flag = true;
    } else if (exe_write_back.rd == register2) {
      control_signals.exe_control.signForward2 = 1;
      flag = true;
    }
    if (flag &&
        exe_mem_control.type == Control::MEMControlInfo::MemType::LOAD) {
      stallPipeLine();
      return;
    }
  }
  {
    std::unique_lock<std::shared_mutex>(rwlock_);
    exe_control_.push_front(control_signals.exe_control);
    mem_control_.push_front(control_signals.mem_control);
    wb_control_.push_front(control_signals.wb_control);
  }
  return control_signals.reg_control;
}

void InsBoard::injectBubble() {
  {
    std::unique_lock<std::shared_mutex>(rwlock_);
    exe_control_.push_front(Control::EXEControlInfo());
    mem_control_.push_front(Control::MEMControlInfo());
    wb_control_.push_front(Control::WBControlInfo());
  }
}
void InsBoard::stallPipeLine() {
  // The bubble has been injected in refreshStage().
  ID_IF_reg_stall.write(1);
}

void InsBoard::flushPipeLine() {
  // The wrong ins is in ID and EXE, and only one need to be disabled is EXE as
  // the ID is bubbled.
  std::unique_lock<std::shared_mutex>(rwlock_);
  mem_control_.write(1).type = Control::MEMControlInfo::MemType::NOP;
  wb_control_.write(1).allow = false;
}

Control::EXEControlInfo InsBoard::readEXEControl() {
  std::shared_lock<std::shared_mutex>(rwlock_);
  return exe_control_[1];
}

Control::MEMControlInfo InsBoard::readMEMControl() {
  std::shared_lock<std::shared_mutex>(rwlock_);
  return mem_control_[2];
}

Control::WBControlInfo InsBoard::readWBControlInfo() {
  std::shared_lock<std::shared_mutex>(rwlock_);
  return wb_control_[3];
}

void InsBoard::refreshStage() {
  injectBubble();
  std::unique_lock<std::shared_mutex>(rwlock_);
  exe_control_.pop_back();
  mem_control_.pop_back();
  wb_control_.pop_back();
}