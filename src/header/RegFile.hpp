#ifndef REGFILE_HPP
#define REGFILE_HPP
#include "utils.hpp"

/*status refreshed by ROB is in start of the circle, while the ins changes it
 * after one circle.*/
struct RegFileItem {
  uint32_t value = 0;
  uint32_t ins_index = 0;
  bool busy = false;
};

class RegFile {
private:
  RegFileItem storage[32];
  RegFile() = default;

public:
  static RegFile &getInstance();
  BusyValue tryRead(uint32_t target);
  BusyValue tryWrite(uint32_t target, uint32_t ins_index);
  void commitReceive(ROBCommitInfo &info);
  void flushRecieve(ROBFlushInfo &info);
  void refresh();
};

#endif