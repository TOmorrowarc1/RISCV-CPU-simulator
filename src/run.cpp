#include "run.hpp"
/*
Wires keep the work inside them, while functions keep the control logic,
quiring information from the InsBoard to arrange the dataflow.
*/

void StageIF() {
  unsigned int target_addr = EXE_IF_reg_target_addr.read();
  if (target_addr != 0) {
    unsigned int refresh_pc = EXE_IF_reg_pc.read();
    bool if_jump = bool(EXE_IF_reg_if_jump.read());
    ProgramCounter::getInstance().refreshBranch(refresh_pc, target_addr,
                                                if_jump);
  }
  auto result = ProgramCounter::getInstance().judge(EXE_IF_reg_branch.read());
  IF_ID_reg_ins.write(result.first);
  IF_ID_reg_pc.write(result.second);
}

void StageID() {
  
}

void StageEXE();
void StageMEM();
void StageWB();