#include "utils.hpp"

bool isBetween(uint32_t front, uint32_t end, uint32_t target) {
  if (front <= end) {
    return target >= front && target <= end;
  }
  return target >= front || target <= end;
}

buffer<BasicInsInfo> Fetch_command;
buffer<BranchPredictInfo> PC_predict;

buffer<ALUInfo> ALU_ready;
buffer<BUInfo> BU_ready;

buffer<BoardCastInfo> ALU_result;
buffer<BoardCastInfo> BU_result;
buffer<BoardCastInfo> LSB_result;

buffer<BoardCastInfo> CDB_result;

buffer<ROBCommitInfo> ROB_commit;
buffer<ROBFlushInfo> ROB_flush;

bool stop_flag = false;
bool ALU_stall = false;
bool BU_stall = false;
bool LSB_stall = false;
bool ROB_stall = false;
bool stall_flag = false;

uint32_t total_branch = 0;
uint32_t wrong_branch = 0;