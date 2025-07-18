#ifndef STORAGE_HPP
#define STORAGE_HPP
#include <mutex>
#include <shared_mutex>

class Register {
private:
  unsigned int content_;

public:
  Register();
  unsigned int read();
  void write(unsigned int value);
};

class RegisterGroup {
private:
  Register registers_[32];
  std::mutex lock_;

  RegisterGroup() {
    for (auto iter : registers_) {
      iter.write(0);
    }
  };

public:
  static RegisterGroup &getInstance();
  unsigned int read(unsigned int target);
  void write(int target, unsigned int value);
};

class StatusRegister {
private:
  Register input_;
  Register output_;
  std::shared_mutex rwlock_;
  bool allow_;

public:
  unsigned int read();
  void write(unsigned int value);
  void setAllow(bool sign);
  void refresh();
};

/*Acturally, every stage register is a group of registers, however, as we
describe the whole control sys in a behavior way, we can ignore most of them
only symbolizing the data use to calculate or be calculated.*/

extern StatusRegister IF_ID_reg_ins; // for instruction
extern StatusRegister IF_ID_reg_pc;  // for PC count

extern StatusRegister ID_EX_reg_1; // for register1
extern StatusRegister ID_EX_reg_2; // for register2

extern StatusRegister EXE_MEM_reg_alu;        // for the oprand of next stages
extern StatusRegister EXE_MEM_reg_reg2;       // for register2 which may store
extern StatusRegister EXE_IF_reg_branch;      // for possible branch miss
extern StatusRegister EXE_IF_reg_pc;          // for BPU refresh.
extern StatusRegister EXE_IF_reg_target_addr; // for BPU refresh.
extern StatusRegister EXE_IF_reg_if_jump;     // for BPU refresh.

extern StatusRegister MEM_WB_reg_1; // for result need be written back

extern bool Stop;

#endif