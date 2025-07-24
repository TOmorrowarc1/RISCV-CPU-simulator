#include "header/Executor.hpp"

ALU &ALU::getInstance() {
  static ALU instance;
  return instance;
}
BoardCastInfo ALU::execute(ALUInfo &order) {
  BoardCastInfo result;
  result.index = order.index;
  switch (order.type) {
  case CalcType::ADD:
    result.value = order.oprand1 + order.oprand2;
    break;
  case CalcType::SUB:
    result.value = order.oprand1 - order.oprand2;
    break;
  case CalcType::SLL:
    result.value = order.oprand1 << order.oprand2;
    break;
  case CalcType::SRA:
    result.value = int(order.oprand1) >> order.oprand2;
    break;
  case CalcType::SRL:
    result.value = order.oprand1 >> order.oprand2;
    break;
  case CalcType::LT:
    result.value = int(order.oprand1) < int(order.oprand2);
    break;
  case CalcType::LTU:
    result.value = order.oprand1 < order.oprand2;
    break;
  case CalcType::XOR:
    result.value = order.oprand1 ^ order.oprand2;
    break;
  case CalcType::OR:
    result.value = order.oprand1 | order.oprand2;
    break;
  case CalcType::AND:
    result.value = order.oprand1 & order.oprand2;
    break;
  case CalcType::IMM:
    result.value = order.oprand2;
    break;
  }
  return result;
}

BU &BU::getInstance() {
  static BU instance;
  return instance;
}

BoardCastInfo BU::execute(BUInfo &order){
  BoardCastInfo result;
  switch(order.type){
    
  }
}

LSB &LSB::getInstance() {
  static LSB instance;
  return instance;
}

void LSB::newIns(DecodeInsInfo &decode, BusyValue &oprand1, BusyValue &oprand2,
                 uint32_t index);
void LSB::listenCDB(BoardCastInfo &info);
BoardCastInfo LSB::tryExecute();
void LSB::flushReceive(ROBFlushInfo &info);
void LSB::refresh();