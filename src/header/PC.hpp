#ifndef PC_HPP
#define PC_HPP
#include "memory.hpp"

const uint32_t BATSIZE = 1 << 20;
const uint32_t COVER = 0xfffff;

class JumpState {
private:
  uint32_t status = 1;
  uint32_t address = 0;

public:
  void refreshCondition(bool if_jump);
  void refreshAddress(uint32_t new_addr);
  uint32_t predict();
};

class PC {
private:
  buffer<uint32_t> pc_;
  JumpState BAT_[BATSIZE];
  PC();

public:
  static PC &getInstance();
  BranchPredictInfo branchPredict();
  BasicInsInfo fetchCommand();
  void flushReceive(ROBFlushInfo &info);
  void refresh();
};

#endif