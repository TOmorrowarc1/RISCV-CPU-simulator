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
  unsigned int branch_miss = EXE_IF_reg_branch.read();
  if (branch_miss != 0) {
    InsBoard::getInstance().flushPipeLine();
    return;
  }
  unsigned int command = IF_ID_reg_ins.read();
  unsigned int pc = IF_ID_reg_pc.read();
  Control::RegControlInfo reg_addr = InsBoard::getInstance().IR(command, pc);
  ID_EX_reg_1.write(RegisterGroup::getInstance().read(reg_addr.register1));
  ID_EX_reg_2.write(RegisterGroup::getInstance().read(reg_addr.register2));
}

void StageEXE() {
  auto exe_control = InsBoard::getInstance().readEXEControl();
  unsigned int oprand1;
  unsigned int oprand2;
  switch (exe_control.signForward1) {
  case 0:
    if (exe_control.signPC) {
      oprand1 = exe_control.pc;
    } else {
      oprand1 = ID_EX_reg_1.read();
    }
  case 1:
    oprand1 = EXE_MEM_reg_alu.read();
    break;
  case 2:
    oprand1 = MEM_WB_reg_1.read();
    break;
  }
  switch (exe_control.signForward2) {
  case 0:
    if (exe_control.signImmediate) {
      oprand2 = exe_control.immdiate;
    } else {
      oprand2 = ID_EX_reg_2.read();
    }
  case 1:
    oprand2 = EXE_MEM_reg_alu.read();
    break;
  case 2:
    oprand2 = MEM_WB_reg_1.read();
    break;
  }
  unsigned int result =
      ALU::getInstance().calculate(exe_control.type, oprand1, oprand2);
  if (exe_control.signResultPC) {
    result = exe_control.pc;
  }
  if (exe_control.branch_addr != 0) {
    EXE_IF_reg_pc.write(exe_control.pc);
    EXE_IF_reg_target_addr.write(exe_control.branch_addr);
    unsigned int correct_addr;
    unsigned int now_addr = IF_ID_reg_pc.read();
    if (result != 0) {
      correct_addr = exe_control.branch_addr;
      EXE_IF_reg_if_jump.write(1);
    } else {
      correct_addr = exe_control.pc + 4;
      EXE_IF_reg_if_jump.write(0);
    }
    if (correct_addr != now_addr) {
      EXE_IF_reg_branch.write(correct_addr);
    }
  }
  EXE_MEM_reg_alu.write(result);
  EXE_MEM_reg_reg2.write(ID_EX_reg_2.read());
}

void StageMEM() {
  auto mem_control = InsBoard::getInstance().readMEMControl();
  unsigned int address = EXE_MEM_reg_alu.read();
  unsigned int data = EXE_MEM_reg_reg2.read();
  unsigned int result;
  if (mem_control.type == Control::MEMControlInfo::MemType::LOAD) {
    result = LSDealer::getInstance().load(mem_control.size, address,
                                          mem_control.extend_sign);
  } else if (mem_control.type == Control::MEMControlInfo::MemType::STORE) {
    LSDealer::getInstance().store(mem_control.size, address, data);
  }
  MEM_WB_reg_1.write(result);
}

void StageWB() {
  auto wb_control = InsBoard::getInstance().readWBControlInfo();
  unsigned int data = MEM_WB_reg_1.read();
  if (wb_control.allow) {
    RegisterGroup::getInstance().write(wb_control.rd, data);
  }
}