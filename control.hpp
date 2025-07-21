#ifndef CONTROL_HPP
#define CONTROL_HPP
#include <stdint.h>
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

struct ControlInfo {
  /*Info used by more than one executor.*/
  enum class InsType { CALC, LOAD, STORE, BRANCH };
  InsType type;
  uint32_t register1 = 0;
  uint32_t register2 = 0;
  uint32_t immdiate = 0;
  uint32_t pc = 0;

  uint32_t rd = 0;
  bool allow = false;

  /*Info used by alu, which will be shaped into ALUControl in stage of issue.*/
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
    IMM
  };
  CalcType calcType = CalcType::ADD;
  bool signPC = false;
  bool signImmediate = false;

  /*Info used by mem, grow into MemControl.*/
  enum class MemType { BYTE, HALF, WORD };
  MemType size = MemType::WORD;
  bool extend_sign = false;

  /*Info used by branch, grow into BranchControl.*/
  enum class BranchType { JAL, JALR, BEQ, BNE, BLT, BGE, BLTU, BGEU };
  BranchType branchType = BranchType::JAL;
  uint32_t predict_target_addr = 0;
  bool predict_taken = 0;
};

ControlInfo parse(uint32_t command, uint32_t pc);
}; // namespace Control

#endif