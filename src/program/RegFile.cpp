#include "RegFile.hpp"

RegFile &RegFile::getInstance() {
  static RegFile instance;
  return instance;
}

BusyValue RegFile::tryRead(uint32_t target) {
  if (target == 0) {
    return {0, false};
  }
  BusyValue result;
  result.busy = storage[target].busy;
  result.value =
      (result.busy) ? storage[target].ins_index : storage[target].value;
  return result;
}

BusyValue RegFile::tryWrite(uint32_t target, uint32_t ins_index) {
  if (target == 0) {
    return {50, false};
  }
  BusyValue result;
  result.busy = storage[target].busy;
  result.value = storage[target].ins_index;
  storage[target].busy = true;
  storage[target].ins_index = ins_index;
  return result;
}

void RegFile::commitReceive(ROBCommitInfo &info) {
  static int count = 0;
  storage[info.rd].value = info.value;
  if (storage[info.rd].ins_index == info.index) {
    storage[info.rd].busy = false;
    storage[info.rd].ins_index = 50;
  }
}

void RegFile::flushRecieve(ROBFlushReg &regs) {
  for (int i = 0; i < 32; ++i) {
    if (regs.recover[i] == ROBSIZE) {
      storage[i].busy = false;
    } else if (regs.recover[i] != ROBSIZE + 1) {
      storage[i].ins_index = regs.recover[i];
    }
  }
}