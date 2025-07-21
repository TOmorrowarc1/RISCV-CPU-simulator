#include "control.hpp"
#include "init.hpp"
#include <iostream>

const int MEMSIZE = 10000;
uint32_t clk = 0;
uint32_t pc = 0;
uint32_t registers[32] = {0};
uint32_t memory[MEMSIZE] = {0};
uint32_t Fetch_Issue_ins = 0;
uint32_t Fetch_Issue_pc = 0;
Control::ControlInfo controlInfo;
uint32_t Execute_result;

void StageIF();
void StageIssue();
void StageExecute();
void StageCommit();

int main() {
  Init();
  while (true) {
    ++clk;
    StageIF();
    StageIssue();
    StageExecute();
    StageCommit();
  }
}

void StageIF(){

}
void StageIssue(){

}
void StageExecute(){

}
void StageCommit(){
  
}