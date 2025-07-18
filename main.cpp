#include "src/run.hpp"
#include <thread>
#include <vector>

int main() {
  while (Stop) {
    std::vector<std::thread> controls;
    controls.push_back(std::thread(StageIF));
    controls.push_back(std::thread(StageID));
    controls.push_back(std::thread(StageEXE));
    controls.push_back(std::thread(StageMEM));
    controls.push_back(std::thread(StageWB));

    for (int i = 0; i < 5; ++i) {
      controls[i].join();
    }

    refreshStage();
  }
}