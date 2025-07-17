#ifndef CONTROL_HPP
#define CONTROL_HPP
#include "mySTL.hpp"
#include <mutex>
#include <shared_mutex>

/*
NOP: empty command.
R-type:ADD,SUB,SLT,SLTU,SLL,SRL,SRA,XOR,OR,AND;
I-type:
  ADDI,SLTI,SLTIU,SLLI,SRLI,SRAI,XORI,ORI,ANDI;
  LW,LH,LHU,LB,LBU;
  JALR;
U-type:
  LUI,AUIPC;
S-type:
  SW,SH,SB;
B-type:
  BEQ,BNE,BGE,BLT,BLTU,BGEU;
J-type:
  JAL;
*/
namespace Control {
struct AllControlInfo {
  RegControlInfo reg_control;
  EXEControlInfo exe_control;
  MEMControlInfo mem_control;
  WBControlInfo wb_control;
};
struct RegControlInfo {
  int register1 = 0;
  int register2 = 0;
};

struct EXEControlInfo {
  /*
  The type refers to the calculation ALU executing, while other signs symbolize
  the source of operands imm-regs or immdiate; forward: 00-regs, 01-EXE_MEM
  forward, 02-MEM-WB forward. OprandBools indicate whether oprand1 is from reg1
  or pc, and whether oprand2 is from reg2 or immdiate, while the Resultbool
  serves for JALR.
  */
  enum class CalcType {
    ADD,
    SUB,
    SLL,
    LT,
    GE,
    LTU,
    GEU,
    EQ,
    NE,
    SRL,
    SRA,
    XOR,
    OR,
    AND,
    PRINTIMM
  };
  enum class JumpType { BYES, BNO, JALR, NOP };
  CalcType type = CalcType::ADD;
  int immdiate = 0;
  JumpType jump = JumpType::NOP;
  int pc = 0;
  int signForward1 = 0;
  int signForward2 = 0;
  bool signPC = false;
  bool signImmediate = false;
  bool signResultPC = false;
};

struct MEMControlInfo {
  /*
  The operator indicates which the stage executes: L/S/NOP, while the sign of
  extend shows whether the result need to be SLL. The size distiguishes the
  result length: Byte/Half/Words.
  */
  enum class MemSize { BYTE, HALF, WORD };
  enum class MemType { LOAD, STORE, NOP };
  MemSize size = MemSize::WORD;
  MemType type = MemType::NOP;
  bool extend_sign = false;
};

struct WBControlInfo {
  bool allow = false;
  int rd = 0;
};
}; // namespace Control

class Instruction {
private:
  unsigned int command_;
  unsigned int pc_next_;
  bool has_jump_;

public:
  Instruction();
  Instruction(unsigned int command);
  Control::AllControlInfo parse();
};

class InsBoard {
private:
  // remember to fill them before start.
  sjtu::queue<Control::RegControlInfo, 4> reg_control_;
  sjtu::queue<Control::EXEControlInfo, 4> exe_control_;
  sjtu::queue<Control::MEMControlInfo, 4> mem_control_;
  sjtu::queue<Control::WBControlInfo, 4> wb_control_;
  std::shared_mutex rwlock_;
  InsBoard();

public:
  static InsBoard &getInstance();
  void IR(Instruction &target);
  void stallPipeLine();
  void flushPipeLine();
  void refreshStage();
  Control::EXEControlInfo readEXEControl();
  Control::MEMControlInfo readMEMControl();
  Control::WBControlInfo readWBControlInfo();
  Control::RegControlInfo readRegControl();
};

#endif