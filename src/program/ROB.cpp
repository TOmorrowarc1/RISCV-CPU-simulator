#include "ROB.hpp"

ROB::ROB() : head_(0), tail_(0) { flush_flag = false; }

bool ROB::empty() {
  return head_.getValue() == tail_.getValue() &&
         !storage[head_.getValue()].busy.getValue();
}
bool ROB::full() {
  return head_.getValue() == tail_.getValue() &&
         storage[head_.getValue()].busy.getValue();
}

uint32_t ROB::front(uint32_t now) {
  return now - 1 < 0 ? ROBSIZE - 1 : now - 1;
}
uint32_t ROB::next(uint32_t now) { return now + 1 == ROBSIZE ? 0 : now + 1; }

ROB &ROB::getInstance() {
  static ROB instance;
  return instance;
}

uint32_t ROB::getTail() { return tail_.getValue(); }

uint32_t ROB::newIns(ROBInsInfo info) {
  if (flush_flag) {
    return 50;
  }
  uint32_t tail_now = tail_.getValue();
  storage[tail_now].busy.writeValue(true);
  storage[tail_now].state.writeValue(false);
  storage[tail_now].rd = info.rd;
  if (info.predict_branch == -114514) {
    storage[tail_now].state.writeValue(true);
  }
  storage[tail_now].origin_index = info.origin_index;
  storage[tail_now].predict_branch = info.predict_branch;
  storage[tail_now].predict_taken = info.predict_taken;
  tail_.writeValue(next(tail_now));
  return tail_now;
}

BusyValue ROB::getOperand(uint32_t index) {
  BusyValue answer;
  if (storage[index].state.getValue()) {
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
  if (storage[info.index].predict_branch != 0) {
    if (storage[info.index].predict_taken != info.flag ||
        storage[info.index].predict_branch != info.branch) {
      flush_flag = true;
      tail_.writeValue(info.index);
      ROBFlushInfo flush_info;
      ROBFlushReg flush_regs;
      uint32_t now_tail = tail_.getValue();
      flush_info.branch = info.branch;
      flush_info.taken = info.flag;
      flush_info.branch_index = info.index;
      flush_info.tail_index = now_tail;
      ROB_flush.writeValue(flush_info);
      for (int i = info.index; i != now_tail; i = next(i)) {
        storage[i].busy.writeValue(false);
      }
      storage[now_tail].busy.writeValue(false);
      for (int i = 0; i < 50; ++i) {
        flush_regs.recover[i] = 50;
      }
      for (int i = now_tail; i != info.index; i = front(i)) {
        flush_regs.recover[storage[i].rd] = storage[i].origin_index;
      }
      ROB_flush_reg.writeValue(flush_regs);
    }
  }
}

ROBCommitInfo ROB::tryCommit() {
  if (flush_flag) {
    return ROBCommitInfo();
  }
  ROBCommitInfo answer;
  uint32_t head_now = head_.getValue();
  answer.index = head_now;
  if (!empty() && storage[head_now].state.getValue()) {
    answer.rd = storage[head_now].rd;
    answer.value = storage[head_now].result;
    storage[head_now].busy.writeValue(false);
    head_.writeValue(next(head_now));
    if (storage[head_now].predict_branch = -114514) {
      stop_flag = true;
    }
  }
  return answer;
}

void ROB::refresh() {
  for (int i = 0; i < ROBSIZE; ++i) {
    storage[i].busy.refresh();
    storage[i].state.refresh();
  }
  head_.refresh();
  tail_.refresh();
  flush_flag = false;
  ROB_commit.writeValue(ROBCommitInfo());
  ROB_flush.writeValue(ROBFlushInfo());
}