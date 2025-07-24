#include "header/RegFile.hpp"

RegFile &RegFile::getInstance() {
  static RegFile instance;
  return instance;
}

BusyValue RegFile::tryRead(uint32_t target) {
  BusyValue result;
  result.busy = storage[target].busy;
  result.value =
      (result.busy) ? storage[target].ins_index : storage[target].value;
}

BusyValue RegFile::tryWrite(uint32_t target, uint32_t ins_index) {
  BusyValue result;
  result.busy = storage[target].busy;
  result.value = storage[target].ins_index;
  storage[target].busy = true;
  storage[target].ins_index = ins_index;
  return result;
}

void RegFile::commitReceive(ROBCommitInfo &info) {
  storage[info.rd].value = info.value;
  storage[info.rd].busy = !(storage[info.rd].ins_index == info.index);
}

void RegFile::flushRecieve(ROBFlushInfo &info) {
  
}

void RegFile::refresh() {}