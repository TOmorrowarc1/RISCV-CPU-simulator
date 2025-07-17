#ifndef STORAGE_HPP
#define STORAGE_HPP
#include <mutex>
#include <shared_mutex>

class Register {
private:
  int content_;

public:
  Register();
  int read();
  void write(int value);
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
  int read(int target);
  int write(int target, int value);
};

class StageRegister {
private:
  Register input_;
  Register output_;
  std::shared_mutex rwlock_;

public:
  int read();
  void write(int value);
  void refresh();
};

/*Acturally, every stage register is a group of registers, however, as we
describe the whole control sys in a behavior way, we can ignore most of them
only symbolizing the data use to calculate or be calculated.*/

extern StageRegister IF_ID_reg_ins;      // for instruction
extern StageRegister IF_ID_reg_pc;       // for PC count
extern StageRegister IF_ID_reg_has_jump; // for jump or not
extern StageRegister ID_IF_reg_stall;    // for hazard

extern StageRegister ID_EX_reg_1; // for register1
extern StageRegister ID_EX_reg_2; // for register2

extern StageRegister EXE_MEM_reg_alu;   // for the oprand of next stages
extern StageRegister EXE_MEM_reg_reg2;  // for register2 which may store
extern StageRegister EXE_IF_reg_branch; // for possible branch miss

extern StageRegister MEM_WB_reg_1; // for result need be written back

#endif