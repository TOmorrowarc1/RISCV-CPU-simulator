#ifndef CDB_HPP
#define CDB_HPP
#include "utils.hpp"

const uint32_t CDBSIZE = 8;
struct CDBItem {
  BoardCastInfo content;
  bool busy = false;
};

class CDBSelector {
private:
  CDBItem storage[CDBSIZE];
  uint32_t head_ = 0;

  CDBSelector() = default;
  uint32_t next(uint32_t now);

public:
  static CDBSelector &getInstance();
  BoardCastInfo tryCommit();
  void newInfo(BoardCastInfo &info);
  void flushReceive(ROBFlushInfo &info);
};
#endif