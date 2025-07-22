#include "control.hpp"
#include "init.hpp"
#include "memory.hpp"
#include <iostream>

extern std::byte memory[MEMSIZE];
std::string fileName =
    "/home/tomorrow_arc1/CS/RISCV-CPU-stimulation/sample/sample.data";
bool flag = true;

uint32_t clk = 0;
uint32_t pc = 0;
uint32_t registers[32] = {0};

uint32_t Fetch_Issue_ins = 0;
uint32_t Fetch_Issue_pc = 0;
Control::ControlInfo controlInfo;
uint32_t Execute_result;

void StageIF();
void StageIssue();
void StageExecute();
void StageCommit();

int main() {
  if (!loadMemory(fileName)) {
    throw std::exception();
  };
  while (flag) {
    ++clk;
    StageIF();
    StageIssue();
    StageExecute();
    StageCommit();
  }
}

void StageIF() {
  Fetch_Issue_ins = Memory::getInstance().load(pc, 4);
  Fetch_Issue_pc = pc;
  pc += 4;
}

void StageIssue() {
  // Just decode. The controlInfo server as a naive ROB: remember everything.
  controlInfo = Control::parse(Fetch_Issue_ins, Fetch_Issue_pc);
}

void StageExecute() {
  // Dispatch and execute.
  if (controlInfo.type == Control::InsType::END) {
    std::cout << (registers[controlInfo.rd] & 0xff);
    flag = false;
    return;
  }
  if (controlInfo.type == Control::InsType::CALC) {
    Control::AluControlInfo aluInfo;
    aluInfo.type = controlInfo.calcType;
    aluInfo.oprand_1 =
        controlInfo.signPC ? pc : registers[controlInfo.register1];
    aluInfo.oprand_2 = controlInfo.signImmediate
                           ? controlInfo.immdiate
                           : registers[controlInfo.register2];
    switch (aluInfo.type) {
    case Control::CalcType::ADD:
      Execute_result = aluInfo.oprand_1 + aluInfo.oprand_2;
      break;
    case Control::CalcType::SUB:
      Execute_result = aluInfo.oprand_1 - aluInfo.oprand_2;
      break;
    case Control::CalcType::SLL:
      Execute_result = aluInfo.oprand_1 << aluInfo.oprand_2;
      break;
    case Control::CalcType::SRA:
      Execute_result = int(aluInfo.oprand_1) >> aluInfo.oprand_2;
      break;
    case Control::CalcType::SRL:
      Execute_result = aluInfo.oprand_1 >> aluInfo.oprand_2;
      break;
    case Control::CalcType::LT:
      Execute_result = int(aluInfo.oprand_1) < int(aluInfo.oprand_2);
      break;
    case Control::CalcType::LTU:
      Execute_result = aluInfo.oprand_1 < aluInfo.oprand_2;
      break;
    case Control::CalcType::XOR:
      Execute_result = aluInfo.oprand_1 ^ aluInfo.oprand_2;
      break;
    case Control::CalcType::OR:
      Execute_result = aluInfo.oprand_1 | aluInfo.oprand_2;
      break;
    case Control::CalcType::AND:
      Execute_result = aluInfo.oprand_1 & aluInfo.oprand_2;
      break;
    case Control::CalcType::IMM:
      Execute_result = aluInfo.oprand_2;
      break;
    }
  } else if (controlInfo.type == Control::InsType::BRANCH) {
    Control::BranchControlInfo branchInfo;
    branchInfo.type = controlInfo.branchType;
    branchInfo.oprand1 = registers[controlInfo.register1];
    branchInfo.oprand2 = registers[controlInfo.register2];
    branchInfo.immdiate = controlInfo.immdiate;
    branchInfo.pc = controlInfo.pc;
    switch (branchInfo.type) {
    case Control::BranchType::BEQ:
      if (branchInfo.oprand1 == branchInfo.oprand2) {
        pc = branchInfo.pc + branchInfo.immdiate;
      }
      break;
    case Control::BranchType::BNE:
      if (branchInfo.oprand1 != branchInfo.oprand2) {
        pc = branchInfo.pc + branchInfo.immdiate;
      }
      break;
    case Control::BranchType::BGE:
      if (int(branchInfo.oprand1) >= int(branchInfo.oprand2)) {
        pc = branchInfo.pc + branchInfo.immdiate;
      }
      break;
    case Control::BranchType::BGEU:
      if (branchInfo.oprand1 >= branchInfo.oprand2) {
        pc = branchInfo.pc + branchInfo.immdiate;
      }
      break;
    case Control::BranchType::BLT:
      if (int(branchInfo.oprand1) < int(branchInfo.oprand2)) {
        pc = branchInfo.pc + branchInfo.immdiate;
      }
      break;
    case Control::BranchType::BLTU:
      if (branchInfo.oprand1 < branchInfo.oprand2) {
        pc = branchInfo.pc + branchInfo.immdiate;
      }
      break;
    case Control::BranchType::JAL:
      Execute_result = branchInfo.pc + 4;
      pc = branchInfo.pc + branchInfo.immdiate;
      break;
    case Control::BranchType::JALR:
      Execute_result = branchInfo.pc + 4;
      pc = branchInfo.oprand1 + branchInfo.immdiate;
      break;
    default:
      throw std::exception();
    }
  } else {
    Control::MemControlInfo memInfo;
    memInfo.type = controlInfo.type;
    memInfo.size = controlInfo.size;
    memInfo.signExtend = controlInfo.signExtend;
    memInfo.oprand1 = registers[controlInfo.register1];
    memInfo.oprand2 = registers[controlInfo.register2];
    memInfo.immdiate = controlInfo.immdiate;
    if (memInfo.type == Control::InsType::LOAD) {
      uint32_t result =
          Memory::getInstance().load(memInfo.oprand1 + memInfo.immdiate, 4);
      int origin_bits;
      switch (memInfo.size) {
      case Control::MemType::BYTE:
        origin_bits = 8;
        result &= 0xff;
        break;
      case Control::MemType::HALF:
        origin_bits = 16;
        result &= 0xffff;
        break;
      case Control::MemType::WORD:
        origin_bits = 32;
        break;
      default:
        throw std::exception();
      }
      if (memInfo.signExtend) {
        result = result << 32 - origin_bits;
        result = int(result) >> origin_bits;
      }
      Execute_result = result;
    } else {
      uint32_t size;
      switch (memInfo.size) {
      case Control::MemType::BYTE:
        size = 1;
        break;
      case Control::MemType::HALF:
        size = 2;
        break;
      case Control::MemType::WORD:
        size = 4;
        break;
      default:
        throw std::exception();
      }
      Memory::getInstance().store(memInfo.oprand1 + memInfo.immdiate, size,
                                  memInfo.oprand2);
    }
  }
}

void StageCommit() {
  if (controlInfo.allow) {
    registers[controlInfo.rd] = Execute_result;
  }
  registers[0] = 0;
}