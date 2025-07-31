#include "run.hpp"
#include <fstream>
#include <sstream>
#include <string>

void loadMemory();

int main() {
  loadMemory();
  uint32_t clock = 0;
  while (!stop_flag) {
    ++clock;
    StageFetch();
    StageCommit();
    StageIssue();
    StageExecute();
    StageBoardcast();
    RefreshStage();
  }
  std::cout << "\nthe error rate of the branch prediction is: "
            << wrong_branch / double(total_branch);
  return 0;
}

void loadMemory() {
  size_t currentAddress = 0;
  std::string line;
  uint32_t data;
  while (std::getline(std::cin, line)) {
    std::stringstream lineStream(line);
    if (line.empty()) {
      break;
    }
    if (line[0] == '@') {
      lineStream.get();
      if (!(lineStream >> std::hex >> currentAddress)) {
        throw std::runtime_error("error: failed to analysis the address.\n");
      }
    } else {
      while (lineStream >> std::hex >> data) {
        Memory::getInstance().store(currentAddress, 1, data);
        ++currentAddress;
      }
    }
  }
  return;
}