#include "init.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdint.h>

bool loadMemory(const std::string &filename) {
  std::fstream input(filename);
  if (!input.is_open()) {
    std::cerr << "error: failed to open the file\n";
    return false;
  }
  size_t currentAddress = 0;
  std::string line;
  uint32_t data;
  while (std::getline(input, line)) {
    std::stringstream lineStream(line);
    if (line[0] == '@') {
      lineStream.get();
      if (!(lineStream >> std::hex >> currentAddress)) {
        std::cerr << "error: failed to analysis the address.\n";
        return false;
      }
    } else {
      while (lineStream >> std::hex >> data) {
        Memory::getInstance().store(currentAddress, Memory::Size::BYTE, data);
        ++currentAddress;
      }
    }
  }
  input.close();
  return true;
}