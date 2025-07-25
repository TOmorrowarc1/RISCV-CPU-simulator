#include "header/memory.hpp"

Memory &Memory::getInstance() {
  static Memory instance;
  return instance;
}

uint32_t Memory::load(uint32_t address, uint32_t size) {
  check_access(address, size);
  uint32_t result = 0;
  std::memcpy(&result, &storage[address], int(size));
  return result;
}

void Memory::store(uint32_t address, uint32_t size, uint32_t data) {
  check_access(address, size);
  std::memcpy(&storage[address], &data, int(size));
}