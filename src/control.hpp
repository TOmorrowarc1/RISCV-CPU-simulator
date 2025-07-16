#ifndef CONTROL_HPP
#define CONTROL_HPP
#include <mutex>
#include <shared_mutex>

namespace Control {

struct FixedControlInfo {
  int register1 = 0;
  int register2 = 0;
  int immdiate = 0;
};

struct EXEControlInfo {
  /*
  The type refers to the calculation ALU executing, while other signs symbolize
  the source of operands imm-regs or immdiate; forward: 00-regs, 01-EXE_MEM
  forward, 02-MEM-WB forward.
  */
  enum class CalcType { ADD, SUB, SLL, SLT, SLTU, SRL, SRA, XOR, OR, AND };
  CalcType type = CalcType::ADD;
  int signForward1 = 0;
  int signForward2 = 0;
  bool signImmediate = false;
};

struct MEMControlInfo {
  /*
  The operator indicates which the stage executes: L/S/NOP, while the sign of
  extend shows whether the result need to be SLL. The size distiguishes the
  result length: Byte/Half/Words.
  */
  int type = 0;
  int size = 0;
  bool sign = false;
};

struct WBControl {
  bool allow = false;
};
}; // namespace Control
class Instruction {
public:
  enum class Name {
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
    NOP,
    ADD,
    SUB,
    SLT,
    SLTU,
    SLL,
    SRL,
    SRA,
    XOR,
    OR,
    AND,
    ADDI,
    SLTI,
    SLTIU,
    SLLI,
    SRLI,
    SRAI,
    XORI,
    ORI,
    ANDI,
    LW,
    LH,
    LHU,
    LB,
    LBU,
    JALR,
    LUI,
    AUIPC,
    SW,
    SH,
    SB,
    BEQ,
    BNE,
    BGE,
    BLT,
    BLTU,
    BGEU,
    JAL,
  };

private:
  unsigned int command_;
  char type_;
  Name name_;
  unsigned int operand_;
  unsigned int func3_;
  unsigned int func7_;
  unsigned int register1_;
  unsigned int register2_;
  unsigned int rd_;
  int immediate_;

public:
  Instruction();
  Instruction(unsigned int command);
  std::pair<char, Name> getType();
  std::pair<unsigned int, unsigned int> getRegs();
  unsigned int getRd();
  int getImmdiate();
  void parse();
};

class InsBoard {
private:
  Instruction instructions_[4];
  std::shared_mutex rwlock_;
  InsBoard();

public:
  static InsBoard &getInstance();
  void insLaunch(Instruction &target);
  Instruction readExecute();
  Instruction readMEM();
  Instruction readWB();
  void refreshStage();
};

// namespace Control

#endif