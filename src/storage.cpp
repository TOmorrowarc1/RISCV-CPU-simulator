#include "storage.hpp"

Register::Register() { content_ = 0; }
unsigned int Register::read() { return content_; }
void Register::write(unsigned int value) { content_ = value; }

RegisterGroup::RegisterGroup() {
  for (auto iter : registers_) {
    iter.write(0);
  }
};

RegisterGroup &RegisterGroup::getInstance() {
  static RegisterGroup instance;
  return instance;
}
unsigned int RegisterGroup::read(unsigned int target) {
  std::unique_lock<std::mutex>(lock_);
  return registers_[target].read();
}
void RegisterGroup::write(int target, unsigned int value) {
  std::unique_lock<std::mutex>(lock_);
  registers_[target].write(value);
}

unsigned int StatusRegister::read() {
  std::shared_lock<std::shared_mutex>(rwlock_);
  return output_.read();
}
void StatusRegister::write(unsigned int value) {
  std::unique_lock<std::shared_mutex>(rwlock_);
  input_.write(value);
}
void StatusRegister::setAllow(bool sign) {
  std::unique_lock<std::shared_mutex>(rwlock_);
  allow_ = sign;
}
void StatusRegister::refresh() {
  std::unique_lock<std::shared_mutex>(rwlock_);
  if (allow_) {
    output_.write(input_.read());
  }
}

StatusRegister IF_ID_reg_ins;      // for instruction
StatusRegister IF_ID_reg_pc;       // for PC count
StatusRegister IF_ID_reg_has_jump; // for jump or not
StatusRegister ID_IF_reg_stall;    // for hazard

StatusRegister ID_EX_reg_1; // for register1
StatusRegister ID_EX_reg_2; // for register2

StatusRegister EXE_MEM_reg_alu;   // for the oprand of next stages
StatusRegister EXE_MEM_reg_reg2;  // for register2 which may store
StatusRegister EXE_IF_reg_branch; // for possible branch miss

StatusRegister MEM_WB_reg_1; // for result need be written back