#include "ROB.hpp"
#include <iostream>

ROB::ROB() : head_(0), tail_(0) {}

int32_t ROB::front(int32_t now) { return now - 1 < 0 ? ROBSIZE - 1 : now - 1; }
int32_t ROB::next(int32_t now) { return now + 1 == ROBSIZE ? 0 : now + 1; }

ROB &ROB::getInstance() {
  static ROB instance;
  return instance;
}

uint32_t ROB::getTail() { return tail_.getValue(); }

void ROB::newIns(ROBInsInfo info) {
  int32_t tail_now = tail_.getValue();
  storage[tail_now].busy.writeValue(true);
  storage[tail_now].state.writeValue(false);
  storage[tail_now].pc = info.pc;
  storage[tail_now].rd = info.rd;
  if (info.predict_branch == ENDPC) {
    storage[tail_now].state.writeValue(true);
  }
  storage[tail_now].origin_index = info.origin_index;
  storage[tail_now].predict_branch = info.predict_branch;
  storage[tail_now].predict_taken = info.predict_taken;
  storage[tail_now].is_branch = info.is_branch;
  tail_.writeValue(next(tail_now));
}

BusyValue ROB::getOperand(uint32_t index) {
  BusyValue answer;
  if (storage[index].busy.getValue() && storage[index].state.getValue()) {
    answer.busy = false;
    answer.value = storage[index].result;
  } else {
    answer.busy = true;
  }
  return answer;
}

ROBFlushReg ROB::flushRegAsk(uint32_t begin, uint32_t end) {
  ROBFlushReg result;
  for (int i = 0; i < 32; ++i) {
    result.recover[i] = ROBSIZE + 1;
  }
  for (int i = end; isBetween(begin, end, i); i = front(i)) {
    if (storage[i].busy.getValue()) {
      result.recover[storage[i].rd] = storage[i].origin_index;
    }
  }
  return result;
}

void ROB::listenCDB(BoardCastInfo &info) {
  if (info.index >= ROBSIZE) {
    return;
  }
  storage[info.index].state.writeValue(true);
  storage[info.index].result = info.value;
  if (info.branch != 0) {
    if (storage[info.index].busy.getTemp() && storage[info.index].is_branch) {
      ++total_branch;
      ROBFlushInfo flush_info;
      flush_info.pc = storage[info.index].pc;
      flush_info.branch = info.branch;
      flush_info.taken = info.flag;
      if (storage[info.index].predict_taken != info.flag) {
        ++wrong_branch;
      }
      if (storage[info.index].predict_taken != info.flag ||
          storage[info.index].predict_branch != info.branch) {
        flush_info.flag = true;
        if (next(info.index) != head_.getValue()) {
          flush_info.branch_index = next(info.index);
          flush_info.final_index = front(head_.getValue());
        }
      }
      ROB_flush.writeValue(flush_info);
    }
  }
}

void ROB::commitReceive(ROBCommitInfo &info) {
  for (int i = head_.getValue(); i != tail_.getValue(); i = next(i)) {
    if (storage[i].origin_index == info.index) {
      storage[i].origin_index = ROBSIZE;
    }
  }
}

ROBCommitInfo ROB::tryCommit() {
  if (ROB_flush.getTemp().branch != 0) {
    return ROBCommitInfo();
  }
  ROBCommitInfo answer;
  int32_t head_now = head_.getValue();
  if (storage[head_now].busy.getTemp() && storage[head_now].state.getTemp()) {
    answer.index = head_now;
    answer.rd = storage[head_now].rd;
    answer.value = storage[head_now].result;
    storage[head_now].busy.writeValue(false);
    if (storage[head_now].predict_branch == ENDPC) {
      stop_flag = true;
    }
    head_.writeValue(next(head_now));
  }
  return answer;
}

void ROB::flushReceive(ROBFlushInfo &info) {
  if (info.branch_index != 50) {
    uint32_t begin = info.branch_index;
    uint32_t end = info.final_index;
    for (int i = begin; isBetween(begin, end, i); i = next(i)) {
      storage[i].busy.writeValue(false);
    }
    tail_.writeValue(begin);
  }
}

bool ROB::fullCheck() {
  int32_t head_now = head_.getTemp();
  return head_now == tail_.getTemp() && storage[head_now].busy.getTemp();
}

void ROB::refresh() {
  head_.refresh();
  tail_.refresh();
  for (int i = 0; i < ROBSIZE; ++i) {
    storage[i].busy.refresh();
    storage[i].state.refresh();
  }
  ROB_commit.writeValue(ROBCommitInfo());
  ROB_flush.writeValue(ROBFlushInfo());
}

void ROB::print_out() {
  for (int i = head_.getTemp(); i != tail_.getTemp(); i = next(i)) {
    std::cout << i << ' ' << storage[i].busy.getTemp() << ' '
              << storage[i].state.getTemp() << ' ' << storage[i].pc << ' '
              << storage[i].rd << ' ' << storage[i].result << '\n';
  }
}