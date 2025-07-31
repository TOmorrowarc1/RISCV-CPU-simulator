#ifndef PREDICT_HPP
#define PREDICT_HPP
#include <stdint.h>

const uint32_t PHTSIZE = 1 << 20;
const uint32_t PHTCOVER = 0xfffff;
const uint32_t BHRSIZE = 1 << 4;
const uint32_t BHRCOVER = 0xf;
const uint32_t GHRCOVER = 0xff;

class TwoBitPredictor {
private:
  uint32_t status_;

public:
  TwoBitPredictor();
  bool predict();
  void refresh(bool taken);
};

class PHTPredictor {
private:
  TwoBitPredictor PHT_[PHTSIZE];

public:
  PHTPredictor() = default;
  bool predict(uint32_t pc);
  void refresh(uint32_t pc, bool taken);
};

class LocalHistoryPredictor {
private:
  class Recorder {
  private:
    TwoBitPredictor record_[BHRSIZE];
    uint32_t BHR_;

  public:
    Recorder();
    bool predict();
    void refresh(bool taken);
  };

  Recorder PHT_[PHTSIZE];

public:
  LocalHistoryPredictor() = default;
  bool predict(uint32_t pc);
  void refresh(uint32_t pc, bool taken);
};

class GlobalHistoryPredictor {
private:
  TwoBitPredictor PHT_[PHTSIZE];
  uint32_t GHR_;

public:
  GlobalHistoryPredictor();
  bool predict(uint32_t pc);
  void refresh(uint32_t pc, bool taken);
};

class TournamentPredictor {
private:
  LocalHistoryPredictor predictor1_;
  GlobalHistoryPredictor predictor2_;
  PHTPredictor meta_predictor_;

public:
  TournamentPredictor() = default;
  bool predict(uint32_t pc);
  void refresh(uint32_t pc, bool taken);
};

class BranchTargetBuffer {
private:
  uint32_t target_[PHTSIZE];

public:
  BranchTargetBuffer();
  uint32_t predict(uint32_t pc);
  void refresh(uint32_t pc, uint32_t addr);
};

#endif