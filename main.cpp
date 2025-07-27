#include "run.hpp"
#include <fstream>
#include <sstream>
#include <string>

bool loadMemory(const std::string &filename);

int main() {
  std::string file_name =
      "/home/tomorrow_arc1/CS/RISCV-CPU-stimulation/testcases/array_test1.data";
  loadMemory(file_name);
  uint32_t clock = 0;
  while (!stop_flag && clock < 100000) {
    ++clock;
    StageFetch();
    StageIssue();
    StageExecute();
    StageBoardcast();
    StageCommit();
    RefreshStage();
  }
  return 0;
}

bool loadMemory(const std::string &filename) {
  std::fstream input(filename);
  if (!input.is_open()) {
    throw std::runtime_error("error: failed to open the file\n");
    return false;
  }
  size_t currentAddress = 0;
  std::string line;
  uint32_t data;
  while (std::getline(input, line)) {
    std::stringstream lineStream(line);
    if (line[0] == '@') {
      lineStream.get();
      if (!(lineStream >> std::hex >> currentAddress)) {
        throw std::runtime_error("error: failed to analysis the address.\n");
        return false;
      }
    } else {
      while (lineStream >> std::hex >> data) {
        Memory::getInstance().store(currentAddress, 1, data);
        ++currentAddress;
      }
    }
  }
  input.close();
  return true;
}