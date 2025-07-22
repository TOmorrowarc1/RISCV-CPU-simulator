#ifndef MEMORY_HPP
#define MEMORY_HPP
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <stdint.h>

const int MEMSIZE = 4096;

class Memory {
public:
  enum Size { BYTE=1, HALF=2, WORD=4};

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
  uint32_t load(uint32_t address, Size size);
  void store(uint32_t address, Size size, uint32_t data);
};
#endif