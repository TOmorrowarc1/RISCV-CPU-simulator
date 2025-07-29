#include "CDBselector.hpp"

uint32_t CDBSelector::next(uint32_t now) {
  return now + 1 == CDBSIZE ? 0 : now + 1;
}

CDBSelector &CDBSelector::getInstance() {
  static CDBSelector instance;
  return instance;
}

void CDBSelector::newInfo(BoardCastInfo &info) {
  if (info.index >= 50) {
    return;
  }
  uint32_t pointer = head_;
  do {
    if (!storage[pointer].busy) {
      storage[pointer].content = info;
      storage[pointer].busy = true;
      return;
    }
    pointer = next(pointer);
  } while (pointer != head_);
  throw std::runtime_error("Too much ins in CDB wait list!");
}

BoardCastInfo CDBSelector::tryCommit() {
  uint32_t pointer = head_;
  if (storage[head_].busy) {
    head_ = next(head_);
    storage[pointer].busy = false;
    return storage[pointer].content;
  }
  pointer = next(pointer);
  while (pointer != head_) {
    if (storage[pointer].busy) {
      storage[pointer].busy = false;
      return storage[pointer].content;
    }
    pointer = next(pointer);
  }
  return BoardCastInfo();
}

void CDBSelector::flushReceive(ROBFlushInfo &info) {
  for (int i = 0; i < CDBSIZE; ++i) {
    if (storage[i].busy) {
      if (isBetween(info.branch_index, info.final_index,
                    storage[i].content.index)) {
        storage[i].busy = false;
      }
    }
  }
}
