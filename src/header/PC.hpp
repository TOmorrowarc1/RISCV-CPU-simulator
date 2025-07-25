#ifndef PC_HPP
#define PC_HPP
#include "memory.hpp"

class PC {
private:
  buffer<uint32_t> pc_;

public:
  static PC &getInstance();
  BranchPredictInfo branchPredict();
  BasicInsInfo fetchCommand(uint32_t address);
  void flushReceive(ROBFlushInfo& info);
  void refresh();
};

#endif