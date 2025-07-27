#include "ROB.hpp"
#include <iostream>

ROB::ROB() : head_(0), tail_(0) { flush_flag = false; }

bool ROB::empty() { return head_ == tail_ && !storage[head_].busy; }
bool ROB::full() { return head_ == tail_ && storage[head_].busy; }

int32_t ROB::front(int32_t now) { return now - 1 < 0 ? ROBSIZE - 1 : now - 1; }
int32_t ROB::next(int32_t now) { return now + 1 == ROBSIZE ? 0 : now + 1; }

ROB &ROB::getInstance() {
  static ROB instance;
  return instance;
}

uint32_t ROB::getTail() { return tail_; }

uint32_t ROB::newIns(ROBInsInfo info) {
  if (flush_flag) {
    return 50;
  }
  uint32_t tail_now = tail_;
  storage[tail_].busy = true;
  storage[tail_].state = false;
  storage[tail_].pc = info.pc;
  storage[tail_].rd = info.rd;
  if (info.predict_branch == -114514) {
    storage[tail_].state = true;
  }
  storage[tail_].origin_index = info.origin_index;
  storage[tail_].predict_branch = info.predict_branch;
  storage[tail_].predict_taken = info.predict_taken;
  tail_ = next(tail_);
  return tail_now;
}

BusyValue ROB::getOperand(uint32_t index) {
  BusyValue answer;
  if (storage[index].busy && storage[index].state) {
    answer.busy = false;
    answer.value = storage[index].result;
  } else if (ROB_commit.getTemp().index == index) {
    answer.busy = false;
    answer.value = ROB_commit.getTemp().value;
  } else {
    answer.busy = true;
  }
  return answer;
}

void ROB::listenCDB(BoardCastInfo info) {
  if (info.index >= 50) {
    return;
  }
  storage[info.index].state = true;
  storage[info.index].result = info.value;
  if (info.branch != 0) {
    if (storage[info.index].busy &&
        (storage[info.index].predict_taken != info.flag ||
         storage[info.index].predict_branch != info.branch)) {
      flush_flag = true;
      ROBFlushInfo flush_info;
      ROBFlushReg flush_regs;
      flush_info.pc = storage[info.index].pc;
      flush_info.branch = info.branch;
      flush_info.taken = info.flag;
      flush_info.branch_index = next(info.index);
      flush_info.tail_index = front(tail_);
      ROB_flush.writeValue(flush_info);
      for (int i = flush_info.branch_index;
           isBetween(flush_info.branch_index, flush_info.tail_index, i);
           i = next(i)) {
        storage[i].busy = false;
      }
      for (int i = 0; i < 32; ++i) {
        flush_regs.recover[i] = 51;
      }
      for (int i = front(tail_); i != info.index; i = front(i)) {
        flush_regs.recover[storage[i].rd] = storage[i].origin_index;
      }
      ROB_flush_reg.writeValue(flush_regs);
      tail_ = next(info.index);
    }
  }
}

ROBCommitInfo ROB::tryCommit() {
  if (flush_flag) {
    return ROBCommitInfo();
  }
  ROBCommitInfo answer;
  if (storage[head_].busy && storage[head_].state) {
    answer.index = head_;
    answer.rd = storage[head_].rd;
    answer.value = storage[head_].result;
    storage[head_].busy = false;
    if (storage[head_].predict_branch == -114514) {
      stop_flag = true;
    }
    head_ = next(head_);
    for (int i = head_; i != tail_; i = next(i)) {
      if (storage[i].origin_index == answer.index) {
        storage[i].origin_index = 50;
      }
    }
  }
  return answer;
}

bool ROB::fullCheck() { return head_ == tail_ && storage[head_].busy; }

void ROB::refresh() {
  flush_flag = false;
  ROB_commit.writeValue(ROBCommitInfo());
  ROB_flush.writeValue(ROBFlushInfo());
  ROB_flush_reg.writeValue(ROBFlushReg());
}

void ROB::print_out() {
  for (int i = head_; i != tail_; i = next(i)) {
    std::cout << i << ' ' << storage[i].busy << ' ' << storage[i].state << ' '
              << storage[i].pc << ' ' << storage[i].rd << ' '
              << storage[i].result << '\n';
  }
}