#ifndef PC_HPP
#define PC_HPP
#include "Predictor.hpp"
#include "memory.hpp"


const uint32_t BATSIZE = 1 << 20;
const uint32_t COVER = 0xfffff;

class PC {
private:
  buffer<uint32_t> pc_;
  TournamentPredictor if_taken_;
  BranchTargetBuffer target_addr_;
  PC();

public:
  static PC &getInstance();
  BranchPredictInfo branchPredict();
  BasicInsInfo fetchCommand();
  void flushReceive(ROBFlushInfo &info);
  void predictReceive(ROBFlushInfo &info);
  void refresh();
};

#endif