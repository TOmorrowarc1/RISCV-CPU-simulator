#include "PC.hpp"

PC::PC() : pc_(0) {}

PC &PC::getInstance() {
  static PC instance;
  return instance;
}

BranchPredictInfo PC::branchPredict() {
  BranchPredictInfo result;
  uint32_t pc = pc_.getValue();
  result.taken_predict = if_taken_.predict(pc);
  result.branch_predict =
      (result.taken_predict) ? target_addr_.predict(pc) : (pc_.getValue() + 4);
  pc_.writeValue(result.branch_predict);
  return result;
}

BasicInsInfo PC::fetchCommand() {
  uint32_t command = Memory::getInstance().load(pc_.getValue(), 4);
  return {command, pc_.getValue()};
}

void PC::flushReceive(ROBFlushInfo &info) {
  pc_.writeValue(info.branch);
  pc_.refresh();
}

void PC::predictReceive(ROBFlushInfo &info) {
  if_taken_.refresh(info.pc, info.taken);
  if (info.taken) {
    target_addr_.refresh(info.pc, info.branch);
  }
}

void PC::refresh() {
  if (!stall_flag) {
    pc_.refresh();
  }
}