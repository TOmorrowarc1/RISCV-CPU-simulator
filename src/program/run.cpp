#include "run.hpp"

void StageFetch() {
  PC_predict.writeValue(PC::getInstance().branchPredict());
  Fetch_command.writeValue(PC::getInstance().fetchCommand());
}

void StageIssue() {
  // Refresh.
  auto commit_info = ROB_commit.getValue();
  auto CDB_info = CDB_result.getValue();
  RegFile::getInstance().commitReceive(commit_info);
  ALU_RS::getInstance().listenCDB(CDB_info);
  BU_RS::getInstance().listenCDB(CDB_info);
  LSB::getInstance().listenCDB(CDB_info);

  auto flush_info = ROB_flush.getValue();
  if (flush_info.branch != 0) {
    auto flush_regs = ROB::getInstance().flushRegAsk(flush_info.branch_index,
                                                     flush_info.final_index);
    RegFile::getInstance().flushRecieve(flush_regs);
    ALU_RS::getInstance().flushReceive(flush_info);
    BU_RS::getInstance().flushReceive(flush_info);
    LSB::getInstance().flushReceive(flush_info);
    ALU_ready.writeValue(ALUInfo());
    BU_ready.writeValue(BUInfo());
    return;
  }

  if (!stall_flag) {
    // Decode.
    auto info = Decoder::getInstance().parse(Fetch_command.getValue(),
                                             PC_predict.getValue());
    uint32_t index = ROB::getInstance().getTail();
    auto oprand1_info = RegFile::getInstance().tryRead(info.register1);
    auto oprand2_info = RegFile::getInstance().tryRead(info.register2);
    if (info.signPC) {
      oprand1_info.busy = false;
      oprand1_info.value = info.pc;
    } else {
      if (oprand1_info.busy) {
        auto oprand1_possi = ROB::getInstance().getOperand(oprand1_info.value);
        if (!oprand1_possi.busy) {
          oprand1_info = oprand1_possi;
        } else if (CDB_info.index == oprand1_info.value) {
          oprand1_info.busy = false;
          oprand1_info.value = CDB_info.value;
        }
      }
    }
    if (info.signImmediate) {
      oprand2_info.busy = false;
      oprand2_info.value = info.immediate;
    } else if (oprand2_info.busy) {
      auto oprand2_possi = ROB::getInstance().getOperand(oprand2_info.value);
      if (!oprand2_possi.busy) {
        oprand2_info = oprand2_possi;
      } else if (CDB_info.index == oprand2_info.value) {
        oprand2_info.busy = false;
        oprand2_info.value = CDB_info.value;
      }
    }
    ROBInsInfo newIns_info;
    newIns_info.pc = info.pc;
    newIns_info.predict_branch = info.predict_target_addr;
    newIns_info.predict_taken = info.predict_taken;
    if (info.allow) {
      if (info.type != InsType::END) {
        auto write_info = RegFile::getInstance().tryWrite(info.rd, index);
        newIns_info.origin_index = write_info.busy ? write_info.value : 50;
      }
      newIns_info.rd = info.rd;
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
  }

  // Launch.
  ALU_ready.writeValue(ALU_RS::getInstance().tryCommit());
  BU_ready.writeValue(BU_RS::getInstance().tryCommit());
}

void StageExecute() {
  auto alu_info = ALU_ready.getValue();
  auto branch_info = BU_ready.getValue();
  auto commit_info = ROB_commit.getValue();
  auto flush_info = ROB_flush.getValue();
  if (flush_info.branch != 0) {
    if (isBetween(flush_info.branch_index, flush_info.final_index,
                  alu_info.index)) {
      ALU_result.writeValue(BoardCastInfo());
    } else {
      ALU_result.writeValue(ALU::getInstance().execute(alu_info));
    }
    if (isBetween(flush_info.branch_index, flush_info.final_index,
                  branch_info.index)) {
      BU_result.writeValue(BoardCastInfo());
    } else {
      BU_result.writeValue(BU::getInstance().execute(branch_info));
    }
    LSB_result.writeValue(BoardCastInfo());
  } else {
    ALU_result.writeValue(ALU::getInstance().execute(alu_info));
    BU_result.writeValue(BU::getInstance().execute(branch_info));
    LSB_result.writeValue(LSB::getInstance().tryExecute(commit_info));
  }
}

void StageBoardcast() {
  auto alu_result = ALU_result.getValue();
  auto branch_result = BU_result.getValue();
  auto ls_result = LSB_result.getValue();
  CDBSelector::getInstance().newInfo(alu_result);
  CDBSelector::getInstance().newInfo(branch_result);
  CDBSelector::getInstance().newInfo(ls_result);
  auto flush_info = ROB_flush.getValue();
  if (flush_info.branch != 0) {
    CDBSelector::getInstance().flushReceive(flush_info);
    CDB_result.writeValue(BoardCastInfo());
    return;
  }
  CDB_result.writeValue(CDBSelector::getInstance().tryCommit());
}

void StageCommit() {
  auto flush_info = ROB_flush.getValue();
  auto CDB_info = CDB_result.getValue();
  ROB::getInstance().listenCDB(CDB_info);
  if (flush_info.branch != 0) {
    ROB::getInstance().flushReceive(flush_info);
  }
  ROB_commit.writeValue(ROB::getInstance().tryCommit());
}

void RefreshStage() {
  LSB_stall = LSB::getInstance().fullCheck();
  ROB_stall = ROB::getInstance().fullCheck();
  stall_flag = ALU_stall || BU_stall || LSB_stall || ROB_stall;
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

  // 旁路：组合逻辑，以新状态更新新状态。
  auto flush = ROB_flush.getValue();
  if (flush.branch != 0) {
    PC::getInstance().flushReceive(flush);
  }

  PC::getInstance().refresh();
  ALU_RS::getInstance().refresh();
  BU_RS::getInstance().refresh();
  LSB::getInstance().refresh();
  ROB::getInstance().refresh();

  print_log();

  if (stop_flag) {
    auto commit_check = ROB_commit.getValue();
    std::cout << (RegFile::getInstance().tryRead(commit_check.rd).value & 0xff);
  }
}

void print_log() {
  auto ins_info = Fetch_command.getValue();
  auto alu_ready = ALU_ready.getValue();
  auto bu_ready = BU_ready.getValue();
  auto alu_result = ALU_result.getValue();
  auto bu_result = BU_result.getValue();
  auto ls_result = LSB_result.getValue();
  auto board = CDB_result.getValue();
  auto commit = ROB_commit.getValue();
  auto flush = ROB_flush.getValue();

  std::cout << "ins_pc: " << ins_info.ins_pc << '\n';
  std::cout << "alu_ready: " << alu_ready.index << '\n';
  std::cout << "bu_ready: " << bu_ready.index << '\n';
  std::cout << "alu_result: " << alu_result.index << '\n';
  std::cout << "bu_result: " << bu_result.index << '\n';
  std::cout << "LSB_result: " << ls_result.index << '\n';
  std::cout << "board_info: " << board.index << '\n';
  std::cout << "commit_info: " << commit.index << '\n';
  std::cout << "flush_info: " << flush.branch_index << ' ' << flush.final_index
            << ' ' << flush.branch << ' ' << flush.taken << '\n';
  LSB::getInstance().print_out();
  ROB::getInstance().print_out();
  std::cout << '\n';
}