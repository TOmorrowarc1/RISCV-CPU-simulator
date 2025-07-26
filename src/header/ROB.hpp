#ifndef ROB_HPP
#define ROB_HPP
#include "utils.hpp"

const int ROBSIZE = 50;

// Items store in ROB, the "state" records if it is complete.
struct ROBItem {
  uint32_t pc = 0;
  uint32_t rd = 0;
  uint32_t origin_index = 0;
  uint32_t result = 0;
  uint32_t predict_branch = 0;
  bool state = false;
  bool busy = false;
  bool predict_taken = false;
};

class ROB {
private:
  ROBItem storage[ROBSIZE];
  int32_t head_;
  int32_t tail_;
  bool flush_flag;

  ROB();
  bool empty();
  bool full();
  int32_t front(int32_t now);
  int32_t next(int32_t now);

public:
  static ROB &getInstance();
  uint32_t getTail();
  uint32_t newIns(ROBInsInfo info);
  BusyValue getOperand(uint32_t index);
  ROBCommitInfo tryCommit();
  void listenCDB(BoardCastInfo info);
  void print_out();
  void refresh();
};

#endif