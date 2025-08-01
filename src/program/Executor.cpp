#include "Executor.hpp"

ALU &ALU::getInstance() {
  static ALU instance;
  return instance;
}

BoardCastInfo ALU::execute(ALUInfo &order) {
  BoardCastInfo result;
  result.index = order.index;
  switch (order.type) {
  case CalcType::ADD:
    result.value = order.oprand1 + order.oprand2;
    break;
  case CalcType::SUB:
    result.value = order.oprand1 - order.oprand2;
    break;
  case CalcType::SLL:
    result.value = order.oprand1 << order.oprand2;
    break;
  case CalcType::SRA:
    result.value = int(order.oprand1) >> order.oprand2;
    break;
  case CalcType::SRL:
    result.value = order.oprand1 >> order.oprand2;
    break;
  case CalcType::LT:
    result.value = int(order.oprand1) < int(order.oprand2);
    break;
  case CalcType::LTU:
    result.value = order.oprand1 < order.oprand2;
    break;
  case CalcType::XOR:
    result.value = order.oprand1 ^ order.oprand2;
    break;
  case CalcType::OR:
    result.value = order.oprand1 | order.oprand2;
    break;
  case CalcType::AND:
    result.value = order.oprand1 & order.oprand2;
    break;
  case CalcType::IMM:
    result.value = order.oprand2;
    break;
  }
  return result;
}

BU &BU::getInstance() {
  static BU instance;
  return instance;
}

BoardCastInfo BU::execute(BUInfo &order) {
  BoardCastInfo result;
  result.index = order.index;
  switch (order.type) {
  case BranchType::BEQ:
    if (order.oprand1 == order.oprand2) {
      result.flag = true;
      result.branch = order.pc + order.immdiate;
    } else {
      result.flag = false;
      result.branch = order.pc + 4;
    }
    break;
  case BranchType::BNE:
    if (order.oprand1 != order.oprand2) {
      result.flag = true;
      result.branch = order.pc + order.immdiate;
    } else {
      result.flag = false;
      result.branch = order.pc + 4;
    }
    break;
  case BranchType::BGE:
    if (int(order.oprand1) >= int(order.oprand2)) {
      result.flag = true;
      result.branch = order.pc + order.immdiate;
    } else {
      result.flag = false;
      result.branch = order.pc + 4;
    }
    break;
  case BranchType::BGEU:
    if (order.oprand1 >= order.oprand2) {
      result.flag = true;
      result.branch = order.pc + order.immdiate;
    } else {
      result.flag = false;
      result.branch = order.pc + 4;
    }
    break;
  case BranchType::BLT:
    if (int(order.oprand1) < int(order.oprand2)) {
      result.flag = true;
      result.branch = order.pc + order.immdiate;
    } else {
      result.flag = false;
      result.branch = order.pc + 4;
    }
    break;
  case BranchType::BLTU:
    if (order.oprand1 < order.oprand2) {
      result.flag = true;
      result.branch = order.pc + order.immdiate;
    } else {
      result.flag = false;
      result.branch = order.pc + 4;
    }
    break;
  case BranchType::JAL:
    result.value = order.pc + 4;
    result.flag = true;
    result.branch = order.pc + order.immdiate;
    break;
  case BranchType::JALR:
    result.value = order.pc + 4;
    result.flag = true;
    result.branch = order.oprand1 + order.immdiate;
    break;
  default:
    throw std::exception();
  }
  return result;
}

LSB::LSB() {
  head_ = buffer<int32_t>(0);
  tail_ = buffer<int32_t>(0);
}

int32_t LSB::front(int32_t now) { return now - 1 < 0 ? LSBSIZE - 1 : now - 1; }
int32_t LSB::next(int32_t now) { return now + 1 == LSBSIZE ? 0 : now + 1; }

LSB &LSB::getInstance() {
  static LSB instance;
  return instance;
}

void LSB::newIns(DecodeInsInfo &decode, BusyValue &oprand1, BusyValue &oprand2,
                 uint32_t index) {
  int32_t tail_now = tail_.getValue();
  storage[tail_now].busy.writeValue(true);
  storage[tail_now].ins_index = index;
  storage[tail_now].type = decode.type;
  switch (decode.size) {
  case MemType::BYTE:
    storage[tail_now].size = 1;
    break;
  case MemType::HALF:
    storage[tail_now].size = 2;
    break;
  case MemType::WORD:
    storage[tail_now].size = 4;
    break;
  }
  storage[tail_now].immediate = decode.immediate;
  storage[tail_now].oprand1 = oprand1.value;
  storage[tail_now].oprand2 = oprand2.value;
  storage[tail_now].ready1 = !oprand1.busy;
  storage[tail_now].ready2 = !oprand2.busy;
  if (storage[tail_now].type == InsType::LOAD) {
    storage[tail_now].ready.writeValue(storage[tail_now].ready1);
  } else {
    storage[tail_now].ready.writeValue(storage[tail_now].ready1 &&
                                       storage[tail_now].ready2);
  }
  if (storage[tail_now].ready1) {
    storage[tail_now].target = storage[tail_now].oprand1 + decode.immediate;
  }
  tail_.writeValue(next(tail_now));
}

void LSB::listenCDB(BoardCastInfo &info) {
  for (int i = 0; i < LSBSIZE; ++i) {
    if (storage[i].busy.getValue() && storage[i].ready.getValue() == 0) {
      if (!storage[i].ready1 && info.index == storage[i].oprand1) {
        storage[i].oprand1 = info.value;
        storage[i].ready1 = true;
      }
      if (!storage[i].ready2 && info.index == storage[i].oprand2) {
        storage[i].oprand2 = info.value;
        storage[i].ready2 = true;
      }
      if (storage[i].type == InsType::LOAD) {
        storage[i].ready.writeValue(storage[i].ready1);
      } else {
        storage[i].ready.writeValue(storage[i].ready1 && storage[i].ready2);
      }
      if (storage[i].ready1) {
        storage[i].target = storage[i].oprand1 + storage[i].immediate;
      }
    }
  }
}

BoardCastInfo LSB::tryExecute(ROBCommitInfo &info) {
  BoardCastInfo result;
  int32_t head_now = head_.getValue();
  if (storage[head_now].busy.getValue()) {
    if (storage[head_now].ready.getValue() == 1) {
      storage[head_now].ready.writeValue(2);
      if (storage[head_now].type == InsType::STORE) {
        result.index = storage[head_now].ins_index;
        result.value = storage[head_now].target;
        return result;
      }
    } else if (storage[head_now].ready.getValue() == 2) {
      if (storage[head_now].type == InsType::LOAD) {
        result.index = storage[head_now].ins_index;
        result.value = Memory::getInstance().load(storage[head_now].target,
                                                  storage[head_now].size);
        storage[head_now].busy.writeValue(false);
        head_.writeValue(next(head_now));
        return result;
      } else {
        if (info.index == storage[head_now].ins_index) {
          Memory::getInstance().store(storage[head_now].target,
                                      storage[head_now].size,
                                      storage[head_now].oprand2);
          storage[head_now].busy.writeValue(false);
          head_.writeValue(next(head_now));
        }
      }
    }
  }
  for (int i = next(head_now); i != head_now; i = next(i)) {
    if (storage[i].busy.getValue() && storage[i].ready.getValue() == 1 &&
        storage[i].type == InsType::STORE) {
      storage[i].ready.writeValue(2);
      result.index = storage[i].ins_index;
      result.value = storage[i].target;
      return result;
    }
  }
  return result;
}

void LSB::flushReceive(ROBFlushInfo &info) {
  int32_t head_now = head_.getValue();
  int32_t tail_now = tail_.getValue();
  int32_t tail_new = head_now;
  for (int i = 0; i < LSBSIZE; ++i) {
    storage[i].busy.writeValue(false);
  }
  while (!isBetween(info.branch_index, info.final_index,
                    storage[tail_new].ins_index) &&
         storage[tail_new].busy.getValue()) {
    storage[tail_new].busy.writeValue(true);
    tail_new = next(tail_new);
  }
  tail_.writeValue(tail_new);
}

bool LSB::fullCheck() {
  return head_.getTemp() == tail_.getTemp() && storage[0].busy.getTemp();
}

void LSB::refresh() {
  for (int i = 0; i < LSBSIZE; ++i) {
    storage[i].busy.refresh();
    storage[i].ready.refresh();
  }
  head_.refresh();
  tail_.refresh();
}

void LSB::print_out() {
  for (int i = head_.getTemp(); i != tail_.getTemp(); i = next(i)) {
    std::cout << storage[i].busy.getTemp() << ' ' << storage[i].ready.getTemp()
              << ' ' << storage[i].ins_index << '\n';
  }
  std::cout << "LSB finish.\n";
}