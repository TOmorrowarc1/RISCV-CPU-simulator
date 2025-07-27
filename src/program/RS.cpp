#include "RS.hpp"

uint32_t ALU_RS::next(uint32_t now) { return now + 1 == RSSIZE ? 0 : now + 1; }

ALU_RS &ALU_RS::getInstance() {
  static ALU_RS instance;
  return instance;
}

void ALU_RS::newIns(DecodeInsInfo &decode, BusyValue &oprand1,
                    BusyValue &oprand2, uint32_t index) {
  bool flag = false;
  int i = head_;
  do {
    if (!storage[i].busy.getValue()) {
      storage[i].busy.writeValue(true);
      storage[i].ready.writeValue(!oprand1.busy && !oprand2.busy);
      storage[i].ins_index = index;
      storage[i].type = decode.calcType;
      storage[i].oprand1 = oprand1.value;
      storage[i].ready1 = !oprand1.busy;
      storage[i].oprand2 = oprand2.value;
      storage[i].ready2 = !oprand2.busy;
      flag = true;
      break;
    }
    i = next(i);
  } while (i != head_);
  if (!flag) {
    throw std::runtime_error("too much ins in RS.");
  }
}

void ALU_RS::listenCDB(BoardCastInfo &info) {
  for (int i = 0; i < RSSIZE; ++i) {
    if (storage[i].busy.getValue()) {
      if (!storage[i].ready1 && info.index == storage[i].oprand1) {
        storage[i].oprand1 = info.value;
        storage[i].ready1 = true;
      }
      if (!storage[i].ready2 && info.index == storage[i].oprand2) {
        storage[i].oprand2 = info.value;
        storage[i].ready2 = true;
      }
      storage[i].ready.writeValue(storage[i].ready1 && storage[i].ready2);
    }
  }
}

ALUInfo ALU_RS::tryCommit() {
  ALUInfo result;
  if (storage[head_].busy.getValue() && storage[head_].ready.getValue()) {
    result.index = storage[head_].ins_index;
    result.oprand1 = storage[head_].oprand1;
    result.oprand2 = storage[head_].oprand2;
    result.type = storage[head_].type;
    storage[head_].busy.writeValue(false);
    head_ = next(head_);
  } else {
    for (int i = next(head_); i != head_; i = next(i)) {
      if (storage[i].busy.getValue() && storage[i].ready.getValue()) {
        result.index = storage[i].ins_index;
        result.oprand1 = storage[i].oprand1;
        result.oprand2 = storage[i].oprand2;
        result.type = storage[i].type;
        storage[i].busy.writeValue(false);
        break;
      }
    }
  }
  return result;
}

void ALU_RS::flushReceive(ROBFlushInfo &info) {
  for (int i = 0; i < RSSIZE; ++i) {
    if (storage[i].busy.getValue() &&
        isBetween(info.branch_index, info.tail_index, storage[i].ins_index)) {
      storage[i].busy.writeValue(false);
    }
  }
}

void ALU_RS::refresh() {
  for (int i = 0; i < RSSIZE; ++i) {
    storage[i].busy.refresh();
    storage[i].ready.refresh();
  }
}

uint32_t BU_RS::next(uint32_t now) { return now + 1 == RSSIZE ? 0 : now + 1; }

BU_RS &BU_RS::getInstance() {
  static BU_RS instance;
  return instance;
}

void BU_RS::newIns(DecodeInsInfo &decode, BusyValue &oprand1,
                   BusyValue &oprand2, uint32_t index) {
  bool flag = false;
  int i = head_;
  do {
    if (!storage[i].busy.getValue()) {
      storage[i].busy.writeValue(true);
      storage[i].type = decode.branchType;
      storage[i].ins_index = index;
      storage[i].pc = decode.pc;
      storage[i].immediate = decode.immediate;
      switch (decode.branchType) {
      case BranchType::JAL:
        storage[i].ready.writeValue(true);
        break;
      case BranchType::JALR:
        storage[i].oprand1 = oprand1.value;
        storage[i].ready1 = !oprand1.busy;
        storage[i].ready2 = true;
        storage[i].ready.writeValue(!oprand1.busy);
        break;
      default:
        storage[i].oprand1 = oprand1.value;
        storage[i].ready1 = !oprand1.busy;
        storage[i].oprand2 = oprand2.value;
        storage[i].ready2 = !oprand2.busy;
        storage[i].ready.writeValue(!oprand1.busy && !oprand2.busy);
      }
      flag = true;
      break;
    }
    i = next(i);
  } while (i != head_);
  if (!flag) {
    throw std::runtime_error("too much ins in RS.");
  }
}

void BU_RS::listenCDB(BoardCastInfo &info) {
  for (int i = 0; i < RSSIZE; ++i) {
    if (storage[i].busy.getValue()) {
      if (!storage[i].ready1 && info.index == storage[i].oprand1) {
        storage[i].oprand1 = info.value;
        storage[i].ready1 = true;
      }
      if (!storage[i].ready2 && info.index == storage[i].oprand2) {
        storage[i].oprand2 = info.value;
        storage[i].ready2 = true;
      }
      storage[i].ready.writeValue(storage[i].ready1 && storage[i].ready2);
    }
  }
}

BUInfo BU_RS::tryCommit() {
  BUInfo result;
  if (storage[head_].busy.getValue() && storage[head_].ready.getValue()) {
    result.index = storage[head_].ins_index;
    result.type = storage[head_].type;
    result.oprand1 = storage[head_].oprand1;
    result.oprand2 = storage[head_].oprand2;
    result.immdiate = storage[head_].immediate;
    result.pc = storage[head_].pc;
    storage[head_].busy.writeValue(false);
  } else {
    for (int i = next(head_); i != head_; i = next(i)) {
      if (storage[i].busy.getValue() && storage[i].ready.getValue()) {
        result.index = storage[i].ins_index;
        result.type = storage[i].type;
        result.oprand1 = storage[i].oprand1;
        result.oprand2 = storage[i].oprand2;
        result.immdiate = storage[i].immediate;
        result.pc = storage[i].pc;
        storage[i].busy.writeValue(false);
        break;
      }
    }
  }
  return result;
}

void BU_RS::flushReceive(ROBFlushInfo &info) {
  for (int i = 0; i < RSSIZE; ++i) {
    if (storage[i].busy.getValue() &&
        isBetween(info.branch_index, info.tail_index, storage[i].ins_index)) {
      storage[i].busy.writeValue(false);
    }
  }
}

void BU_RS::refresh() {
  for (int i = 0; i < RSSIZE; ++i) {
    storage[i].busy.refresh();
    storage[i].ready.refresh();
  }
}
