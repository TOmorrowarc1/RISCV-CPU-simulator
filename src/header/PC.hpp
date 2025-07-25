#ifndef PC_HPP
#define PC_HPP
#include "memory.hpp"

class PC {
private:
  buffer<uint32_t> pc_;
  PC();

public:
  static PC &getInstance();
  BranchPredictInfo branchPredict();
  BasicInsInfo fetchCommand();
  void flushReceive(ROBFlushInfo &info);
  void refresh();
};

#endif