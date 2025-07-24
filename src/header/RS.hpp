#ifndef RS_HPP
#define RS_HPP
#include "utils.hpp"

const uint32_t RSSIZE = 8;
struct ALU_RS_Item {
  CalcType type;
  uint32_t ins_index;
  uint32_t oprand1;
  uint32_t oprand2;
  buffer<bool> ready = buffer<bool>(false);
  buffer<bool> busy = buffer<bool>(false);
  bool ready1 = false;
  bool ready2 = false;
};

struct BU_RS_Item {
  BranchType type;
  uint32_t ins_index;
  uint32_t oprand1;
  uint32_t oprand2;
  uint32_t immediate;
  uint32_t pc;
  buffer<bool> ready = buffer<bool>(false);
  buffer<bool> busy = buffer<bool>(false);
  bool ready1 = false;
  bool ready2 = false;
};

class ALU_RS {
private:
  ALU_RS_Item storage[RSSIZE];
  bool flush_flag = false;
  ALU_RS() = default;

public:
  static ALU_RS &getInstance();
  void newIns(DecodeInsInfo &decode, BusyValue &oprand1, BusyValue &oprand2,
              uint32_t index);
  void listenCDB(BoardCastInfo &info);
  ALUInfo tryCommit();
  void flushReceive(ROBFlushInfo &info);
  void refresh();
};

class BU_RS {
private:
  BU_RS_Item storage[RSSIZE];
  bool flush_flag = false;
  BU_RS() = default;

public:
  static BU_RS &getInstance();
  void newIns(DecodeInsInfo &decode, BusyValue &oprand1, BusyValue &oprand2,
              uint32_t index);
  void listenCDB(BoardCastInfo &info);
  BUInfo tryCommit();
  void flushReceive(ROBFlushInfo &info);
  void refresh();
};
#endif