#include "header/run.hpp"

int main() {
  uint32_t clock = 0;
  while (!stop_flag) {
    StageFetch();
    StageIssue();
    StageExecute();
    StageBoardcast();
    StageCommit();
    RefreshStage();
  }
  return;
}