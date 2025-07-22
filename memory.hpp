#ifndef MEMORY_HPP
#define MEMORY_HPP
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <stdint.h>

const int MEMSIZE = 4194304; // 4 MB

class Memory {
private:
  std::byte storage[MEMSIZE];
  Memory() { std::memset(storage, 0, MEMSIZE); }

  void check_access(uint32_t address, uint32_t size) const {
    if (storage == nullptr || address + size > MEMSIZE) {
      throw std::runtime_error("Memory access out of bounds!");
    }
    if (address % size != 0) {
      throw std::runtime_error("Misaligned memory access!");
    }
  }

public:
  static Memory &getInstance();
  uint32_t load(uint32_t address, uint32_t size);
  void store(uint32_t address, uint32_t size, uint32_t data);
};
#endif