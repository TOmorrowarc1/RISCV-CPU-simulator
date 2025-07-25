#ifndef ROB_HPP
#define ROB_HPP
#include "utils.hpp"

const int ROBSIZE = 50;

// Items store in ROB, the "state" records if it is complete.
struct ROBItem {
  uint32_t rd = 0;
  uint32_t origin_index = 0;
  uint32_t result = 0;
  uint32_t predict_branch = 0;
  buffer<bool> state = buffer<bool>(false);
  buffer<bool> busy = buffer<bool>(false);
  bool predict_taken = false;
};

class ROB {
private:
  ROBItem storage[ROBSIZE];
  buffer<int> head_;
  buffer<int> tail_;
  bool flush_flag;

  ROB();
  bool empty();
  bool full();
  uint32_t front(uint32_t now);
  uint32_t next(uint32_t now);

public:
  static ROB &getInstance();
  uint32_t getTail();
  uint32_t newIns(ROBInsInfo info);
  BusyValue getOperand(uint32_t index);
  ROBCommitInfo tryCommit();
  void listenCDB(BoardCastInfo info);
  void refresh();
};

#endif