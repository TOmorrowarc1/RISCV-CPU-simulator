#ifndef CONTROL_HPP
#define CONTROL_HPP
#include <mutex>
#include <shared_mutex>
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
  bool signForward();
  bool signHarzard();
  bool signBranch();
};

#endif