#ifndef EXECUTE_HPP
#define EXECUTE_HPP
#include "utils.hpp"

const uint32_t LSBSIZE = 8;
class ALU {
private:
  ALU();

public:
  static ALU &getInstance();
  BoardCastInfo execute(ALUInfo &order);
};

class BU {
private:
  BU();

public:
  static BU &getInstance();
  BoardCastInfo execute(BUInfo &order);
};

/*"ready"
For load: just 2 status 0 and 1, symbolizing ready or not.
For store: 2 status but different dealing ways--0 get ready the same, but 1 need to execute.
*/
struct LSBItem {
  InsType type;
  MemType size;
  uint32_t oprand1;
  uint32_t oprand2;
  uint32_t immediate;
  uint32_t target;
  buffer<uint32_t> ready = buffer<uint32_t>(false);
  buffer<bool> busy = buffer<bool>(false);
  bool ready1 = false;
  bool ready2 = false;
};

class LSB {
private:
  LSBItem storage[LSBSIZE];
  buffer<uint32_t> head_;
  buffer<uint32_t> tail_;
  LSB();

public:
  static LSB &getInstance();
  void newIns(DecodeInsInfo &decode, BusyValue &oprand1, BusyValue &oprand2,
              uint32_t index);
  void listenCDB(BoardCastInfo &info);
  BoardCastInfo tryExecute();
  void flushReceive(ROBFlushInfo &info);
  void refresh();
};

#endif
