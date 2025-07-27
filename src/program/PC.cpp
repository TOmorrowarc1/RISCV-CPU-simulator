#include "PC.hpp"

PC::PC() : pc_(0) {}

PC &PC::getInstance() {
  static PC instance;
  return instance;
}

BranchPredictInfo PC::branchPredict() {
  BranchPredictInfo result;
  uint32_t pc = pc_.getValue();
  uint32_t branch = BAT_[pc & COVER].predict();
  result.branch_predict = (branch == 0) ? (pc_.getValue() + 4) : branch;
  result.taken_predict = !(branch == 0);
  pc_.writeValue(result.branch_predict);
  return result;
}

BasicInsInfo PC::fetchCommand() {
  uint32_t command = Memory::getInstance().load(pc_.getValue(), 4);
  return {command, pc_.getValue()};
}

void PC::flushReceive(ROBFlushInfo &info) {
  pc_.writeValue(info.branch);
  BAT_[info.pc & COVER].refreshAddress(info.branch);
  BAT_[info.pc & COVER].refreshCondition(info.taken);
}

void PC::refresh() {
  if (!stall_flag) {
    pc_.refresh();
  }
}

void JumpState::refreshCondition(bool if_jump) {
  if (if_jump) {
    status = status != 3 ? status + 1 : status;
  } else {
    status = status != 0 ? status - 1 : status;
  }
}

void JumpState::refreshAddress(uint32_t new_addr) { address = new_addr; }

uint32_t JumpState::predict() { return (status > 1) ? address : 0; }