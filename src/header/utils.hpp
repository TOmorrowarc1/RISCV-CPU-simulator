#ifndef UTILS_HPP
#define UTILS_HPP
#include <iostream>
#include <stdexcept>
#include <stdint.h>

template <typename T> class buffer {
private:
  T now_value_;
  T next_value_;

public:
  buffer() = default;
  buffer(T init) { now_value_ = next_value_ = init; }
  T getValue() { return now_value_; }
  T getTemp() { return next_value_; }
  void writeValue(T target) { next_value_ = target; }
  void refresh() { now_value_ = next_value_; }
};

struct BusyValue {
  uint32_t value = 0;
  bool busy = false;
};

struct BasicInsInfo {
  uint32_t command = 0;
  uint32_t ins_pc = 0;
};

struct BranchPredictInfo {
  uint32_t branch_predict = 0;
  uint32_t taken_predict = 0;
};

enum class InsType { CALC, LOAD, STORE, BRANCH, END };
enum class CalcType { ADD, SUB, SLL, LT, LTU, SRL, SRA, XOR, OR, AND, IMM };
enum class MemType { BYTE, HALF, WORD };
enum class BranchType { JAL, JALR, BEQ, BNE, BLT, BGE, BLTU, BGEU };

struct DecodeInsInfo {
  /*Info used by more than one executor.*/
  InsType type = InsType::CALC;
  uint32_t register1 = 0;
  uint32_t register2 = 0;
  uint32_t immediate = 0;
  uint32_t pc = 0;
  uint32_t rd = 0;
  bool allow = false;

  /*Info used by alu, which will be shaped into ALUControl in stage of issue.*/
  CalcType calcType = CalcType::ADD;
  bool signPC = false;
  bool signImmediate = false;

  /*Info used by mem, grow into MemControl.*/
  MemType size = MemType::WORD;
  bool signExtend = false;

  /*Info used by branch, grow into BranchControl.*/
  BranchType branchType = BranchType::JAL;
  uint32_t predict_target_addr = 0;
  bool predict_taken = 0;
};

struct ALUInfo {
  uint32_t index = 50;
  CalcType type = CalcType::ADD;
  uint32_t oprand1 = 0;
  uint32_t oprand2 = 0;
};

struct BUInfo {
  uint32_t index = 50;
  BranchType type = BranchType::BEQ;
  uint32_t oprand1 = 0;
  uint32_t oprand2 = 0;
  uint32_t immdiate = 0;
  uint32_t pc = 0;
};

struct LSBInfo {
  uint32_t index = 50;
  InsType type = InsType::LOAD;
  MemType size = MemType::BYTE;
  uint32_t immdiate = 0;
  uint32_t oprand1 = 0;
  uint32_t oprand2 = 0;
  bool signExtend = false;
};

struct BoardCastInfo {
  uint32_t index = 50;
  uint32_t value = 0;
  uint32_t branch = 0;
  bool flag = false;
};

struct ROBInsInfo {
  uint32_t pc = 0;
  uint32_t rd = 0;
  uint32_t origin_index = 50;
  uint32_t predict_branch = 0;
  bool predict_taken = false;
};

struct ROBCommitInfo {
  uint32_t index = 50;
  uint32_t rd = 0;
  uint32_t value = 0;
};

struct ROBFlushInfo {
  uint32_t pc = 0;
  uint32_t branch = 0;
  uint32_t branch_index = 50;
  uint32_t final_index = 50;
  bool taken = false;
};

struct ROBFlushReg {
  uint32_t recover[32];
};

bool isBetween(uint32_t front, uint32_t end, uint32_t target);

extern buffer<BasicInsInfo> Fetch_command;
extern buffer<BranchPredictInfo> PC_predict;

extern buffer<ALUInfo> ALU_ready;
extern buffer<BUInfo> BU_ready;

extern buffer<BoardCastInfo> ALU_result;
extern buffer<BoardCastInfo> BU_result;
extern buffer<BoardCastInfo> LSB_result;

extern buffer<BoardCastInfo> CDB_result;

extern buffer<ROBCommitInfo> ROB_commit;
extern buffer<ROBFlushInfo> ROB_flush;

extern bool stall_flag;
extern bool ALU_stall;
extern bool BU_stall;
extern bool LSB_stall;
extern bool ROB_stall;
extern bool stop_flag;

constexpr int32_t ROBSIZE = 50;
constexpr uint32_t ENDPC = -114514;

#endif