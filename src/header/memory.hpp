#ifndef MEMORY_HPP
#define MEMORY_HPP
#include "utils.hpp"
#include <cstddef>
#include <cstring>

const int MEMSIZE = 4194304; // 4 MB

class Memory {
private:
  std::byte storage[MEMSIZE];
  Memory() { std::memset(storage, 0, MEMSIZE); }

  void check_access(uint32_t address, uint32_t size) const {
    if (storage == nullptr || address + size > MEMSIZE) {
      throw std::runtime_error("Memory access out of bounds!");
    }
  }

public:
  static Memory &getInstance();
  uint32_t load(uint32_t address, uint32_t size);
  void store(uint32_t address, uint32_t size, uint32_t data);
};
#endif