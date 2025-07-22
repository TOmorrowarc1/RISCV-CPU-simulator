#include "init.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdint.h>

bool loadMemory(const std::string &filename, uint32_t *memory, uint32_t size) {
  std::fstream input(filename);
  if (!input.is_open()) {
    std::cerr << "error: failed to open the file\n";
    return false;
  }
  size_t currentAddress = 0;
  std::string line;
  uint32_t byteValue;
  while (std::getline(input, line)) {
    std::stringstream lineStream(line);
    if (line[0] == '@') {
      lineStream.get();
      if (!(lineStream >> std::hex >> currentAddress)) {
        std::cerr << "error: failed to analysis the address.\n";
        return false;
      }
    } else {
      while (lineStream >> std::hex >> byteValue) {
        if (currentAddress >= size) {
          std::cerr << "error: visit memory out bound\n";
          return false;
        }
        memory[currentAddress] = byteValue;
        ++currentAddress;
      }
    }
  }
  input.close();
  return true;
}