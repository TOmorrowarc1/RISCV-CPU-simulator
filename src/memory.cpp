#include "memory.hpp"

Memory &Memory::getDataInstance() {
  static Memory instance;
  return instance;
}

Memory &Memory::getInsInstance() {
  static Memory instance;
  return instance;
}

unsigned int Memory::load(unsigned int address) { return memory_[address]; }

void Memory::store(unsigned int address, unsigned int content) {
  memory_[address] = content;
}

LSDealer &LSDealer::getInstance() {
  static LSDealer instance;
  return instance;
}

unsigned int LSDealer::load(Control::MEMControlInfo::MemSize size,
                            unsigned int address, bool extend) {
  unsigned int result = Memory::getDataInstance().load(address);
  int origin_bits;
  switch (size) {
  case Control::MEMControlInfo::MemSize::BYTE:
    origin_bits = 8;
    result &= 0xff;
    break;
  case Control::MEMControlInfo::MemSize::HALF:
    origin_bits = 16;
    result &= 0xffff;
    break;
  case Control::MEMControlInfo::MemSize::WORD:
    origin_bits = 32;
    break;
  default:
    throw std::exception();
  }
  if (extend) {
    result = result << 32 - origin_bits;
    result = int(result) >> origin_bits;
  }
  return result;
}

void LSDealer::store(Control::MEMControlInfo::MemSize size,
                     unsigned int address, unsigned int data) {
  switch (size) {
  case Control::MEMControlInfo::MemSize::BYTE:
    data &= 0xff;
    break;
  case Control::MEMControlInfo::MemSize::HALF:
    data &= 0xffff;
    break;
  case Control::MEMControlInfo::MemSize::WORD:
    break;
  default:
    throw std::exception();
  }
  Memory::getDataInstance().store(address, data);
}