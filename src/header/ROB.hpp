#ifndef ROB_HPP
#define ROB_HPP
#include "utils.hpp"

const int ROBSIZE = 50;

struct ROBCommitInfo {
  uint32_t index = 0;
  uint32_t rd = 0;
  uint32_t value = 0;
};

// Items store in ROB, the "state" records if it is complete.
struct ROBItem {
  uint32_t rd = 0;
  uint32_t result = 0;
  uint32_t predict_branch = 0;
  buffer<bool> state = buffer<bool>(false);
  bool predict_taken = false;
};

class ROB {
private:
  ROBItem storage[ROBSIZE];
  buffer<int> head_;
  buffer<int> tail_;

  ROB();
  bool empty();
  uint32_t next(uint32_t now);

public:
  static ROB &getInstance();
  uint32_t newIns(ROBItem info);
  BusyValue getOperand(uint32_t index);
  ROBCommitInfo tryCommit();
  void listenCDB(BoardCastInfo info);
};

#endif