#include "storage.hpp"

Register::Register() { Content = 0; }
int Register::read() { return Content; }
void Register::write(int value) { Content = value; }

RegisterGroup::RegisterGroup() {
  for (auto iter : Registers) {
    iter.write(0);
  }
};

RegisterGroup &RegisterGroup::getInstance() {
  static RegisterGroup instance;
  return instance;
}

int RegisterGroup::read(int target) { return Registers[target].read(); }
int RegisterGroup::write(int target, int value) {
  Registers[target].write(value);
}

int StageRegister::read() { return output.read(); }
void StageRegister::write(int value) { input.write(value); }
void StageRegister::refresh() { output.write(input.read()); }
