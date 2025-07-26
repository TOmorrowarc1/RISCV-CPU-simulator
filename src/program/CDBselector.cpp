#include "CDBselector.hpp"

CDBSelector &CDBSelector::getInstance() {
  static CDBSelector instance;
  return instance;
}

BoardCastInfo CDBSelector::tryCommit() {
  for (int i = 0; i < CDBSIZE; ++i) {
    if (storage[i].busy) {
      storage[i].busy = false;
      return storage[i].content;
    }
  }
  return BoardCastInfo();
}

void CDBSelector::newInfo(BoardCastInfo &info) {
  if (info.index >= 50) {
    return;
  }
  for (int i = 0; i < CDBSIZE; ++i) {
    if (!storage[i].busy) {
      storage[i].content = info;
      storage[i].busy = true;
      return;
    }
  }
  throw std::runtime_error("Too much ins in CDB wait list!");
}

void CDBSelector::flushReceive(ROBFlushInfo &info) {
  for (int i = 0; i < CDBSIZE; ++i) {
    if (storage[i].busy) {
      if (isBetween(info.branch_index, info.tail_index,
                    storage[i].content.index)) {
        storage[i].busy = false;
      }
    }
  }
}
