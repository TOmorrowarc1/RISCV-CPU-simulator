#include "storage.hpp"

Register::Register() { content_ = 0; }
int Register::read() { return content_; }
void Register::write(int value) { content_ = value; }

RegisterGroup::RegisterGroup() {
  for (auto iter : registers_) {
    iter.write(0);
  }
};

RegisterGroup &RegisterGroup::getInstance() {
  static RegisterGroup instance;
  return instance;
}

int RegisterGroup::read(int target) { return registers_[target].read(); }
int RegisterGroup::write(int target, int value) {
  registers_[target].write(value);
}

int StageRegister::read() { return output_.read(); }
void StageRegister::write(int value) { input_.write(value); }
void StageRegister::refresh() { output_.write(input_.read()); }
