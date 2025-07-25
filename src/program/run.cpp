#include "run.hpp"
#include <iostream>

void StageFetch() {
  PC_predict.writeValue(PC::getInstance().branchPredict());
  Fetch_command.writeValue(PC::getInstance().fetchCommand());
  if (ROB_flush.getValue().branch != 0) {
    auto flush_info = ROB_flush.getValue();
    PC::getInstance().flushReceive(flush_info);
  }
}

void StageIssue() {
  if (ROB_flush.getValue().branch != 0) {
    auto flush_info = ROB_flush.getValue();
    auto flush_regs = ROB_flush_reg.getValue();
    RegFile::getInstance().flushRecieve(flush_regs);
    ALU_RS::getInstance().flushReceive(flush_info);
    BU_RS::getInstance().flushReceive(flush_info);
    LSB::getInstance().flushReceive(flush_info);
    return;
  }

  // Refresh.
  auto CDB_info = CDB_result.getValue();
  auto commit_info = ROB_commit.getValue();
  ALU_RS::getInstance().listenCDB(CDB_info);
  BU_RS::getInstance().listenCDB(CDB_info);
  LSB::getInstance().listenCDB(CDB_info);
  RegFile::getInstance().commitReceive(commit_info);

  // Decode.
  auto info = Decoder::getInstance().parse(Fetch_command.getValue(),
                                           PC_predict.getValue());
  uint32_t index = ROB::getInstance().getTail();
  auto oprand1_info = RegFile::getInstance().tryRead(info.register1);
  auto oprand2_info = RegFile::getInstance().tryRead(info.register2);
  if (oprand1_info.busy) {
    oprand1_info = ROB::getInstance().getOperand(oprand1_info.value);
  }
  if (info.signImmediate) {
    oprand2_info.busy = false;
    oprand2_info.value = info.immediate;
  } else if (oprand2_info.busy) {
    oprand2_info = ROB::getInstance().getOperand(oprand2_info.value);
  }
  ROBInsInfo newIns_info;
  newIns_info.rd = info.rd;
  newIns_info.predict_branch = info.predict_target_addr;
  newIns_info.predict_taken = info.predict_taken;
  if (info.type != InsType::END) {
    auto write_info = RegFile::getInstance().tryWrite(info.rd, index);
    newIns_info.origin_index = write_info.busy ? write_info.value : 50;
  }
  ROB::getInstance().newIns(newIns_info);

  // Dispatch.
  switch (info.type) {
  case InsType::CALC:
    ALU_RS::getInstance().newIns(info, oprand1_info, oprand2_info, index);
    break;
  case InsType::BRANCH:
    BU_RS::getInstance().newIns(info, oprand1_info, oprand2_info, index);
    break;
  case InsType::LOAD:
    LSB::getInstance().newIns(info, oprand1_info, oprand2_info, index);
    break;
  case InsType::STORE:
    LSB::getInstance().newIns(info, oprand1_info, oprand2_info, index);
    break;
  default:
    break;
  }

  // Launch.
  ALU_ready.writeValue(ALU_RS::getInstance().tryCommit());
  BU_ready.writeValue(BU_RS::getInstance().tryCommit());
}

void StageExecute() {
  if (ROB_flush.getValue().branch != 0) {
    return;
  }
  auto alu_info = ALU_ready.getValue();
  auto branch_info = BU_ready.getValue();
  auto commit_info = ROB_commit.getValue();
  ALU_result.writeValue(ALU::getInstance().execute(alu_info));
  BU_result.writeValue(BU::getInstance().execute(branch_info));
  LSB_result.writeValue(LSB::getInstance().tryExecute(commit_info));
}

void StageBoardcast() {
  if (ROB_flush.getValue().branch != 0) {
    auto flush_info = ROB_flush.getValue();
    CDBSelector::getInstance().flushReceive(flush_info);
    return;
  }
  auto alu_result = ALU_result.getValue();
  auto branch_result = BU_result.getValue();
  auto ls_result = LSB_result.getValue();
  CDBSelector::getInstance().newInfo(alu_result);
  CDBSelector::getInstance().newInfo(branch_result);
  CDBSelector::getInstance().newInfo(ls_result);
  CDB_result.writeValue(CDBSelector::getInstance().tryCommit());
}

void StageCommit() {
  auto CDB_info = CDB_result.getValue();
  ROB::getInstance().listenCDB(CDB_info);
  ROB::getInstance().tryCommit();
}

void RefreshStage() {
  PC::getInstance().refresh();
  ALU_RS::getInstance().refresh();
  BU_RS::getInstance().refresh();
  LSB::getInstance().refresh();
  CDBSelector::getInstance().refresh();
  ROB::getInstance().refresh();

  Fetch_command.refresh();
  PC_predict.refresh();
  ALU_ready.refresh();
  BU_ready.refresh();
  ALU_result.refresh();
  BU_result.refresh();
  LSB_result.refresh();
  CDB_result.refresh();
  ROB_commit.refresh();
  ROB_flush.refresh();
  ROB_flush_reg.refresh();

  if (stop_flag) {
    auto commit_check = ROB_commit.getValue();
    std::cout << RegFile::getInstance().tryRead(commit_check.rd).value;
  }
}