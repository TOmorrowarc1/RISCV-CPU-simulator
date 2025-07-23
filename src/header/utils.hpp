#ifndef UTILS_HPP
#define UTILS_HPP
#include <stdint.h>

template <typename T> class buffer {
private:
  T now_value_;
  T next_value_;

public:
  buffer() = default;
  buffer(T init) { now_value_ = next_value_ = init; }
  T getValue() { return now_value_; }
  void writeValue(T target) { next_value_ = target; }
  void refresh() { now_value_ = next_value_; }
};

struct BusyValue {
  uint32_t value = 0;
  bool busy = false;
};

struct BoardCastInfo {
  uint32_t index;
  uint32_t value;
  bool flag;
};

struct ROBCommitInfo;

extern buffer<ROBCommitInfo> ROB_commit;

#endif