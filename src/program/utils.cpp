#include "utils.hpp"

bool isBetween(uint32_t front, uint32_t end, uint32_t target) {
  if (front <= end) {
    return target >= front && target <= end;
  }
  return target >= front || target <= end;
}
