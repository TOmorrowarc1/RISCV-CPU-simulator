#include "header/PC.hpp"

PC::PC() : pc_(0) {}

PC &PC::getInstance() {
  static PC instance;
  return instance;
}

BranchPredictInfo PC::branchPredict() {
  BranchPredictInfo result;
  result.branch_predict = pc_.getValue();
  result.taken_predict = false;
  return result;
}

BasicInsInfo PC::fetchCommand(uint32_t address) {
  uint32_t command = Memory::getInstance().load(address, 4);
  return {command, pc_.getValue()};
}

void PC::flushReceive(ROBFlushInfo &info) { pc_.writeValue(info.branch); }

void PC::refresh() { pc_.refresh(); }