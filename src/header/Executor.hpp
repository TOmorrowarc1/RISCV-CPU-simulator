#ifndef EXECUTE_HPP
#define EXECUTE_HPP
#include "memory.hpp"

const uint32_t LSBSIZE = 20;
class ALU {
private:
  ALU() = default;

public:
  static ALU &getInstance();
  BoardCastInfo execute(ALUInfo &order);
};

class BU {
private:
  BU() = default;

public:
  static BU &getInstance();
  BoardCastInfo execute(BUInfo &order);
};

/*"ready"
For load: just 2 status 0 and 1, symbolizing ready or not.
For store: 2 status but different dealing ways--0 get ready the same, but 1 need
to execute.
*/
struct LSBItem {
  InsType type;
  uint32_t size;
  uint32_t ins_index;
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
  buffer<int32_t> head_;
  buffer<int32_t> tail_;

  LSB();
  int32_t front(int32_t now);
  int32_t next(int32_t now);

public:
  static LSB &getInstance();
  void newIns(DecodeInsInfo &decode, BusyValue &oprand1, BusyValue &oprand2,
              uint32_t index);
  void listenCDB(BoardCastInfo &info);
  BoardCastInfo tryExecute(ROBCommitInfo &info);
  void flushReceive(ROBFlushInfo &info);
  bool fullCheck();
  void refresh();
};

#endif
