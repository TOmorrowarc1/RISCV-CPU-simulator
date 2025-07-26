#include "Decoder.hpp"

Decoder &Decoder::getInstance() {
  static Decoder instance;
  return instance;
}

DecodeInsInfo Decoder::parse(BasicInsInfo ins, BranchPredictInfo branch) {
  DecodeInsInfo result;
  result.pc = ins.ins_pc;
  result.register1 = (ins.command >> 15) & 0x1f;
  result.register2 = (ins.command >> 20) & 0x1f;
  result.rd = (ins.command >> 7) & 0x1f;
  result.predict_target_addr = branch.branch_predict;
  result.predict_taken = branch.taken_predict;
  uint32_t opcode = ins.command & 0x7f;
  switch (opcode) {
  case 0x33: {
    // For type 'R': record two target arch regs and the target reg.
    result.type = InsType::CALC;
    result.allow = true;
    uint32_t func3_ = (ins.command >> 12) & 0x7;
    uint32_t func7_ = (ins.command >> 25) & 0x7f;
    switch (func3_) {
    case 0x0:
      if (func7_ == 0x00) {
        result.calcType = CalcType::ADD;
      } else if (func7_ == 0x20) {
        result.calcType = CalcType::SUB;
      } else {
        throw std::exception();
      }
      break;
    case 0x1:
      result.calcType = CalcType::SLL;
      break;
    case 0x2:
      result.calcType = CalcType::LT;
      break;
    case 0x3:
      result.calcType = CalcType::LTU;
      break;
    case 0x4:
      result.calcType = CalcType::XOR;
      break;
    case 0x5:
      if (func7_ == 0x00) {
        result.calcType = CalcType::SRL;
      } else if (func7_ == 0x20) {
        result.calcType = CalcType::SRA;
      } else {
        throw std::exception();
      }
      break;
    case 0x6:
      result.calcType = CalcType::OR;
      break;
    case 0x7:
      result.calcType = CalcType::AND;
      break;
    default:
      break;
    }
    break;
  }
  case 0x13: {
    // for type 'I': immediate calculation.
    result.type = InsType::CALC;
    result.signImmediate = true;
    result.immediate = int(ins.command) >> 20;
    result.allow = true;
    uint32_t func3_ = (ins.command >> 12) & 0x7;
    uint32_t func7_ = (ins.command >> 25) & 0x7f;
    switch (func3_) {
    case 0x0:
      result.calcType = CalcType::ADD;
      if (result.immediate == 255) {
        result.type = InsType::END;
        result.predict_target_addr = -114514;
      }
      break;
    case 0x1:
      result.calcType = CalcType::SLL;
      break;
    case 0x2:
      result.calcType = CalcType::LT;
      break;
    case 0x3:
      result.calcType = CalcType::LTU;
      break;
    case 0x4:
      result.calcType = CalcType::XOR;
      break;
    case 0x5:
      result.immediate = result.immediate & 0x1f;
      if (func7_ == 0x00) {
        result.calcType = CalcType::SRL;
      } else if (func7_ == 0x20) {
        result.calcType = CalcType::SRA;
      } else {
        throw std::exception();
      }
      break;
    case 0x6:
      result.calcType = CalcType::OR;
      break;
    case 0x7:
      result.calcType = CalcType::AND;
      break;
    default:
      throw std::exception();
    }
    break;
  }
  case 0x3: {
    // for type 'I': load reg1+imm into reg2.
    result.type = InsType::LOAD;
    result.immediate = int(ins.command) >> 20;
    result.allow = true;
    result.rd = (ins.command >> 7) & 0x1f;
    uint32_t func3_ = (ins.command >> 12) & 0x7;
    switch (func3_) {
    case 0x0:
      result.size = MemType::BYTE;
      result.signExtend = true;
      break;
    case 0x1:
      result.size = MemType::HALF;
      result.signExtend = true;
      break;
    case 0x2:
      result.size = MemType::WORD;
      result.signExtend = true;
      break;
    case 0x4:
      result.size = MemType::BYTE;
      result.signExtend = false;
      break;
    case 0x5:
      result.size = MemType::HALF;
      result.signExtend = false;
      break;
    default:
      throw std::exception();
    }
    break;
  }
  case 0x67: {
    // for type 'I': (JALR) pc=reg1+imm, rd=pc+4, black sheep No.2.
    result.type = InsType::BRANCH;
    result.branchType = BranchType::JALR;
    result.immediate = int(ins.command) >> 20;
    result.allow = true;
    break;
  }
  case 0x23: {
    // for type 'S': store reg2 into reg1+imm;
    result.type = InsType::STORE;
    result.immediate =
        int(((((ins.command >> 25) & 0x7f) << 5) | ((ins.command >> 7) & 0x1f))
            << 20) >>
        20;
    uint32_t func3_ = (ins.command >> 12) & 0x7;
    switch (func3_) {
    case 0x0:
      result.size = MemType::BYTE;
      break;
    case 0x1:
      result.size = MemType::HALF;
      break;
    case 0x2:
      result.size = MemType::WORD;
      break;
    default:
      throw std::exception();
    }
    break;
  }
  case 0x63: {
    /* for type 'B': conditional branch: examine the prediction.*/
    result.type = InsType::BRANCH;
    result.immediate = int(((((ins.command >> 7) & 0x1) << 11) |
                            (((ins.command >> 8) & 0xf) << 1) |
                            (((ins.command >> 25) & 0x3f) << 5) |
                            (((ins.command >> 31) & 0x1) << 12))
                           << 19) >>
                       19;
    uint32_t func3_ = (ins.command >> 12) & 0x7;
    switch (func3_) {
    case 0x0:
      result.branchType = BranchType::BEQ;
      break;
    case 0x1:
      result.branchType = BranchType::BNE;
      break;
    case 0x4:
      result.branchType = BranchType::BLT;
      break;
    case 0x5:
      result.branchType = BranchType::BGE;
      break;
    case 0x6:
      result.branchType = BranchType::BLTU;
      break;
    case 0x7:
      result.branchType = BranchType::BGEU;
      break;
    default:
      throw std::exception();
    }
    break;
  }
  case 0x37: {
    // for type 'U': LUI ins-ALU do nothing.
    result.type = InsType::CALC;
    result.calcType = CalcType::IMM;
    result.signImmediate = true;
    result.immediate = ins.command & 0xfffff000;
    result.allow = true;
    break;
  }
  case 0x17: {
    // for type 'U': (AUIPC) rd=PC+imm.
    result.type = InsType::CALC;
    result.calcType = CalcType::ADD;
    result.signPC = true;
    result.signImmediate = true;
    result.immediate = ins.command & 0xfffff000;
    result.allow = true;
    break;
  }
  case 0x6f: {
    /* for type 'J': JAL: just write the rd.*/
    result.type = InsType::BRANCH;
    result.branchType = BranchType::JAL;
    result.allow = true;
    result.signImmediate = true;
    result.immediate = int(((((ins.command >> 12) & 0xff) << 12) |
                            (((ins.command >> 20) & 0x1) << 11) |
                            (((ins.command >> 21) & 0x3ff) << 1) |
                            (((ins.command >> 31) & 1) << 20))
                           << 11) >>
                       11;
    break;
  }
  default: {
    break;
  }
  }
  return result;
}