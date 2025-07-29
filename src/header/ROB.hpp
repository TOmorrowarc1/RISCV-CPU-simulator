#ifndef ROB_HPP
#define ROB_HPP
#include "utils.hpp"

/*Items store in ROB, the "state" records if it is complete.
The decode stage use old values in buffers, including new_ins, getOprands and
get tails, while commit stage use old values for flush and commit. However, we
need overcome the problem that the stage change the same value including tail_
and storage[tail_].busy. The solution to the problem is the flush_flag which
ensure the result of the flush can cover the new_ins. But the recovery of the
regs still acts as an obstacle.*/
struct ROBItem {
  uint32_t pc = 0;
  uint32_t rd = 0;
  uint32_t origin_index = 0;
  uint32_t result = 0;
  uint32_t predict_branch = 0;
  buffer<bool> state = false;
  buffer<bool> busy = false;
  bool predict_taken = false;
};

class ROB {
private:
  ROBItem storage[50];
  buffer<int32_t> head_;
  buffer<int32_t> tail_;
  bool flush_flag;

  ROB();
  bool empty();
  bool full();
  int32_t front(int32_t now);
  int32_t next(int32_t now);

public:
  static ROB &getInstance();
  uint32_t getTail();
  void newIns(ROBInsInfo info);
  BusyValue getOperand(uint32_t index);
  ROBCommitInfo tryCommit();
  void listenCDB(BoardCastInfo info);
  bool fullCheck();
  void refresh();
  void print_out();
};

#endif