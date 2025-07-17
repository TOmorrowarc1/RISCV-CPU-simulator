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
int RegisterGroup::read(int target) {
  std::unique_lock<std::mutex>(lock_);
  return registers_[target].read();
}
int RegisterGroup::write(int target, int value) {
  std::unique_lock<std::mutex>(lock_);
  registers_[target].write(value);
}

int StageRegister::read() {
  std::shared_lock<std::shared_mutex>(rwlock_);
  return output_.read();
}
void StageRegister::write(int value) {
  std::unique_lock<std::shared_mutex>(rwlock_);
  input_.write(value);
}
void StageRegister::refresh() {
  std::unique_lock<std::shared_mutex>(rwlock_);
  output_.write(input_.read());
}

StageRegister IF_ID_reg_ins;      // for instruction
StageRegister IF_ID_reg_pc;       // for PC count
StageRegister IF_ID_reg_has_jump; // for jump or not
StageRegister ID_IF_reg_stall;    // for hazard

StageRegister ID_EX_reg_1; // for register1
StageRegister ID_EX_reg_2; // for register2

StageRegister EXE_MEM_reg_alu;   // for the oprand of next stages
StageRegister EXE_MEM_reg_reg2;  // for register2 which may store
StageRegister EXE_IF_reg_branch; // for possible branch miss

StageRegister MEM_WB_reg_1; // for result need be written back