#include "control.hpp"
#include <exception>

Control::ControlInfo Control::parse(uint32_t command, uint32_t pc) {
  Control::ControlInfo result;
  result.pc = pc;
  result.register1 = (command >> 15) & 0x1f;
  result.register2 = (command >> 20) & 0x1f;
  result.rd = (command >> 7) & 0x1f;
  uint32_t opcode = command & 0x7f;
  switch (opcode) {
  case 0x33: {
    // For type 'R': record two target arch regs and the target reg.
    result.type = Control::InsType::CALC;
    result.allow = true;
    uint32_t func3_ = (command >> 12) & 0x7;
    uint32_t func7_ = (command >> 25) & 0x7f;
    switch (func3_) {
    case 0x0:
      if (func7_ == 0x00) {
        result.calcType = Control::CalcType::ADD;
      } else if (func7_ == 0x20) {
        result.calcType = Control::CalcType::SUB;
      } else {
        throw std::exception();
      }
      break;
    case 0x1:
      result.calcType = Control::CalcType::SLL;
      break;
    case 0x2:
      result.calcType = Control::CalcType::LT;
      break;
    case 0x3:
      result.calcType = Control::CalcType::LTU;
      break;
    case 0x4:
      result.calcType = Control::CalcType::XOR;
      break;
    case 0x5:
      if (func7_ == 0x00) {
        result.calcType = Control::CalcType::SRL;
      } else if (func7_ == 0x20) {
        result.calcType = Control::CalcType::SRA;
      } else {
        throw std::exception();
      }
      break;
    case 0x6:
      result.calcType = Control::CalcType::OR;
      break;
    case 0x7:
      result.calcType = Control::CalcType::AND;
      break;
    default:
      throw std::exception();
    }
    break;
  }
  case 0x13: {
    // for type 'I': immediate calculation.
    result.type = Control::InsType::CALC;
    result.signImmediate = true;
    result.immdiate = int(command) >> 20;
    result.allow = true;
    uint32_t func3_ = (command >> 12) & 0x7;
    uint32_t func7_ = (command >> 25) & 0x7f;
    switch (func3_) {
    case 0x0:
      result.calcType = Control::CalcType::ADD;
      if (result.immdiate == 255) {
        result.type = Control::InsType::END;
      }
      break;
    case 0x1:
      result.calcType = Control::CalcType::SLL;
      break;
    case 0x2:
      result.calcType = Control::CalcType::LT;
      break;
    case 0x3:
      result.calcType = Control::CalcType::LTU;
      break;
    case 0x4:
      result.calcType = Control::CalcType::XOR;
      break;
    case 0x5:
      func7_ = (command >> 27) & 0x1f;
      result.immdiate = result.immdiate & 0x1f;
      if (func7_ == 0x0) {
        result.calcType = Control::CalcType::SRL;
      } else if (func7_ == 0x8) {
        result.calcType = Control::CalcType::SRA;
      } else {
        throw std::exception();
      }
      break;
    case 0x6:
      result.calcType = Control::CalcType::OR;
      break;
    case 0x7:
      result.calcType = Control::CalcType::AND;
      break;
    default:
      throw std::exception();
    }
    break;
  }
  case 0x3: {
    // for type 'I': load reg2 into reg1+imm.
    result.type = Control::InsType::LOAD;
    result.immdiate = int(command) >> 20;
    result.allow = true;
    result.rd = (command >> 7) & 0x1f;
    uint32_t func3_ = (command >> 12) & 0x7;
    switch (func3_) {
    case 0x0:
      result.size = Control::MemType::BYTE;
      result.signExtend = true;
      break;
    case 0x1:
      result.size = Control::MemType::HALF;
      result.signExtend = true;
      break;
    case 0x2:
      result.size = Control::MemType::WORD;
      result.signExtend = true;
      break;
    case 0x4:
      result.size = Control::MemType::BYTE;
      result.signExtend = false;
      break;
    case 0x5:
      result.size = Control::MemType::HALF;
      result.signExtend = false;
      break;
    default:
      throw std::exception();
    }
    break;
  }
  case 0x67: {
    // for type 'I': (JALR) pc=reg1+imm, rd=pc+4, black sheep No.2.
    result.type = Control::InsType::BRANCH;
    result.branchType = Control::BranchType::JALR;
    result.immdiate = int(command) >> 20;
    result.allow = true;
    break;
  }
  case 0x23: {
    // for type 'S': store reg2 into reg1+imm;
    result.type = Control::InsType::STORE;
    result.immdiate =
        int(((((command >> 25) & 0x7f) << 5) | ((command >> 7) & 0x1f))
            << 20) >>
        20;
    uint32_t func3_ = (command >> 12) & 0x7;
    switch (func3_) {
    case 0x0:
      result.size = Control::MemType::BYTE;
      break;
    case 0x1:
      result.size = Control::MemType::HALF;
      break;
    case 0x2:
      result.size = Control::MemType::WORD;
      break;
    default:
      throw std::exception();
    }
    break;
  }
  case 0x63: {
    /* for type 'B': conditional branch: examine the prediction.*/
    result.type = Control::InsType::BRANCH;
    result.immdiate =
        int(((((command >> 7) & 0x1) << 11) | (((command >> 8) & 0xf) << 1) |
             (((command >> 25) & 0x3f) << 5) | (((command >> 31) & 0x1) << 12))
            << 19) >>
        19;
    uint32_t func3_ = (command >> 12) & 0x7;
    switch (func3_) {
    case 0x0:
      result.branchType = Control::BranchType::BEQ;
      break;
    case 0x1:
      result.branchType = Control::BranchType::BNE;
      break;
    case 0x4:
      result.branchType = Control::BranchType::BLT;
      break;
    case 0x5:
      result.branchType = Control::BranchType::BGE;
      break;
    case 0x6:
      result.branchType = Control::BranchType::BLTU;
      break;
    case 0x7:
      result.branchType = Control::BranchType::BGEU;
      break;
    default:
      throw std::exception();
    }
    break;
  }
  case 0x37: {
    // for type 'U': LUI ins-ALU do nothing.
    result.type = Control::InsType::CALC;
    result.calcType = Control::CalcType::IMM;
    result.signImmediate = true;
    result.immdiate = command & 0xfffff000 << 12;
    result.allow = true;
    break;
  }
  case 0x17: {
    // for type 'U': (AUIPC) rd=PC+imm.
    result.type = Control::InsType::CALC;
    result.calcType = Control::CalcType::ADD;
    result.signPC = true;
    result.signImmediate = true;
    result.immdiate = command & 0xfffff000 << 12;
    result.allow = true;
    break;
  }
  case 0x6f: {
    /* for type 'J': JAL: just write the rd.*/
    result.type = Control::InsType::BRANCH;
    result.branchType = Control::BranchType::JAL;
    result.allow = true;
    uint32_t immdiate =
        int(((((command >> 12) & 0xff) << 12) |
             (((command >> 20) & 0x1) << 11) |
             (((command >> 21) & 0x3ff) << 1) | (((command >> 31) & 1) << 20))
            << 11) >>
        11;
    result.predict_taken = true;
    result.predict_target_addr = result.pc + immdiate;
  }
  default: {
    throw std::exception();
  }
  }
  return result;
}