#include "ROB.hpp"
#include <iostream>

ROB::ROB() : head_(0), tail_(0) { flush_flag = false; }

int32_t ROB::front(int32_t now) { return now - 1 < 0 ? ROBSIZE - 1 : now - 1; }
int32_t ROB::next(int32_t now) { return now + 1 == ROBSIZE ? 0 : now + 1; }

ROB &ROB::getInstance() {
  static ROB instance;
  return instance;
}

uint32_t ROB::getTail() { return tail_.getValue(); }

void ROB::newIns(ROBInsInfo info) {
  if (flush_flag) {
    auto flush_regs = ROB_flush_reg.getTemp();
    if (info.rd != 0) {
      flush_regs.recover[info.rd] = info.origin_index;
    }
    ROB_flush_reg.writeValue(flush_regs);
    return;
  }
  int32_t tail_now = tail_.getValue();
  storage[tail_now].busy = true;
  storage[tail_now].state = false;
  storage[tail_now].pc = info.pc;
  storage[tail_now].rd = info.rd;
  if (info.pc == ENDPC) {
    storage[tail_now].state = true;
  }
  storage[tail_now].origin_index = info.origin_index;
  storage[tail_now].predict_branch = info.predict_branch;
  storage[tail_now].predict_taken = info.predict_taken;
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

void ROB::listenCDB(BoardCastInfo info) {
  if (info.index >= 50) {
    return;
  }
  storage[info.index].state.writeValue(true);
  storage[info.index].result = info.value;
  if (info.branch != 0) {
    if (storage[info.index].busy.getValue() &&
        (storage[info.index].predict_taken != info.flag ||
         storage[info.index].predict_branch != info.branch)) {
      flush_flag = true;
      ROBFlushInfo flush_info;
      ROBFlushReg flush_regs;
      flush_info.pc = storage[info.index].pc;
      flush_info.branch = info.branch;
      flush_info.taken = info.flag;
      flush_info.branch_index = next(info.index);
      flush_info.final_index = front(head_.getValue());
      ROB_flush.writeValue(flush_info);
      for (int i = 0; i < 32; ++i) {
        flush_regs.recover[i] = ROBSIZE + 1;
      }
      for (int i = flush_info.final_index; i != info.index; i = front(i)) {
        if (storage[i].busy.getTemp()) {
          flush_regs.recover[storage[i].rd] = storage[i].origin_index;
        }
      }
      ROB_flush_reg.writeValue(flush_regs);
      for (int i = flush_info.branch_index;
           isBetween(flush_info.branch_index, flush_info.final_index, i);
           i = next(i)) {
        storage[i].busy.writeValue(false);
      }
      tail_.writeValue(next(info.index));
    }
  }
}

ROBCommitInfo ROB::tryCommit() {
  if (flush_flag) {
    return ROBCommitInfo();
  }
  ROBCommitInfo answer;
  int32_t head_now = head_.getValue();
  if (storage[head_now].busy.getTemp() && storage[head_now].state.getTemp()) {
    answer.index = head_now;
    answer.rd = storage[head_now].rd;
    answer.value = storage[head_now].result;
    storage[head_now].busy = false;
    if (storage[head_now].predict_branch == -114514) {
      stop_flag = true;
    }
    head_.writeValue(next(head_now));
    for (int i = head_now; i != tail_.getTemp(); i = next(i)) {
      if (storage[i].origin_index == answer.index) {
        storage[i].origin_index = ROBSIZE;
      }
    }
  }
  return answer;
}

bool ROB::fullCheck() {
  int32_t head_now = head_.getTemp();
  return head_now == tail_.getTemp() && storage[head_now].busy.getTemp();
}

void ROB::refresh() {
  flush_flag = false;
  ROB_commit.writeValue(ROBCommitInfo());
  ROB_flush.writeValue(ROBFlushInfo());
  ROB_flush_reg.writeValue(ROBFlushReg());
}

void ROB::print_out() {
  for (int i = head_.getTemp(); i != tail_.getTemp(); i = next(i)) {
    std::cout << i << ' ' << storage[i].busy.getTemp() << ' '
              << storage[i].state.getTemp() << ' ' << storage[i].pc << ' '
              << storage[i].rd << ' ' << storage[i].result << '\n';
  }
}