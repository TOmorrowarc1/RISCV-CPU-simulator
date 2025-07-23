#include "header/ROB.hpp"

ROB::ROB() : head_(0), tail_(0) {}

bool ROB::empty() {
  return head_.getValue() == tail_.getValue() &&
         !storage[head_.getValue()].busy.getValue();
}
bool ROB::full() {
  return head_.getValue() == tail_.getValue() &&
         storage[head_.getValue()].busy.getValue();
}

uint32_t ROB::next(uint32_t now) { return now + 1 == ROBSIZE ? 0 : now + 1; }

ROB &ROB::getInstance() {
  static ROB instance;
  return instance;
}

uint32_t ROB::newIns(ROBItem info) {
  uint32_t tail_now = tail_.getValue();
  tail_.writeValue(next(tail_now));
  storage[tail_now] = info;
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
  if (info.index < 50) {
    storage[info.index].state.writeValue(true);
    storage[info.index].result = info.value;
  }
}

ROBCommitInfo ROB::tryCommit() {
  ROBCommitInfo answer;
  uint32_t head_now = head_.getValue();
  answer.index = head_now;
  if (!empty() && storage[head_now].state.getValue()) {
    answer.rd = storage[head_now].rd;
    answer.value = storage[head_now].result;
    storage[head_now].busy.writeValue(false);
    head_.writeValue(next(head_now));
  }
  return answer;
}

void ROB::refresh() {
  for (int i = 0; i < ROBSIZE; ++i) {
    storage[i].busy.refresh();
    storage[i].state.refresh();
  }
}