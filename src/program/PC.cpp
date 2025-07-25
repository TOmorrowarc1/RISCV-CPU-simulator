#include "PC.hpp"

PC::PC() : pc_(0) {}

PC &PC::getInstance() {
  static PC instance;
  return instance;
}

BranchPredictInfo PC::branchPredict() {
  BranchPredictInfo result;
  uint32_t pc_next = pc_.getValue() + 4;
  result.branch_predict = pc_.getValue() + 4;
  result.taken_predict = false;
  pc_.writeValue(pc_next);
  return result;
}

BasicInsInfo PC::fetchCommand() {
  uint32_t command = Memory::getInstance().load(pc_.getValue(), 4);
  return {command, pc_.getValue()};
}

void PC::flushReceive(ROBFlushInfo &info) { pc_.writeValue(info.branch); }

void PC::refresh() { pc_.refresh(); }