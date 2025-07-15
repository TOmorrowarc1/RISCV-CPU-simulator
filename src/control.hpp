#ifndef CONTROL_HPP
#define CONTROL_HPP
#include <mutex>
#include <shared_mutex>
class Instruction {
public:
  enum class Type {
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
    BGT,
    BLTU,
    BGEU,
    JAL,
  };

private:
  unsigned int command;
  Type instructionType;
  unsigned int operand;
  unsigned int register1;
  unsigned int register2;
  unsigned int rd;
  unsigned int immediate;
  unsigned int func3;
  unsigned int func7;

public:
  Instruction();
  Instruction(unsigned int command);
  void parse();
};

class InsBoard {
private:
  Instruction Instructions[4];
  std::shared_mutex RWlock;
  InsBoard();

public:
  static InsBoard &getInstance();
  void insLaunch(Instruction &target);
  Instruction readExecute();
  Instruction readMEM();
  Instruction readWB();
  void refreshStage();
};

#endif