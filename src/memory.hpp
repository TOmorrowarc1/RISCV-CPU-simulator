#ifndef MEM_HPP
#define MEM_HPP
#include "control.hpp"

const int MEMORY_SIZE = 65536;

class Memory {
private:
  unsigned int memory_[MEMORY_SIZE];
  Memory() = default;

public:
  static Memory &getDataInstance();
  unsigned int load(unsigned int address);
  void store(unsigned int address, unsigned int content);
};

class LSDealer {
private:
  LSDealer();

public:
  LSDealer &getInstance();
  unsigned int load(Control::MEMControlInfo::MemSize size, unsigned int address,
                    bool extend);
  void store(Control::MEMControlInfo::MemSize size, unsigned int address,
             unsigned int data);
};
#endif