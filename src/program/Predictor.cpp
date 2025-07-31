#include "Predictor.hpp"

TwoBitPredictor::TwoBitPredictor() { status_ = 1; }
bool TwoBitPredictor::predict() { return status_ > 1; }
void TwoBitPredictor::refresh(bool taken) {
  status_ = taken ? (status_ == 3 ? 3 : status_ + 1)
                  : (status_ == 0 ? 0 : status_ - 1);
}

bool PHTPredictor::predict(uint32_t pc) { return PHT_[pc].predict(); }
void PHTPredictor::refresh(uint32_t pc, bool taken) { PHT_[pc].refresh(taken); }

LocalHistoryPredictor::Recorder::Recorder() { BHR_ = 0; }
bool LocalHistoryPredictor::Recorder::predict() {
  return record_[BHR_].predict();
}
void LocalHistoryPredictor::Recorder::refresh(bool taken) {
  record_[BHR_].refresh(taken);
  BHR_ = ((BHR_ << 1) | (taken)) & BHRCOVER;
}

bool LocalHistoryPredictor::predict(uint32_t pc) { return PHT_[pc].predict(); }

void LocalHistoryPredictor::refresh(uint32_t pc, bool taken) {
  PHT_[pc].refresh(taken);
}

GlobalHistoryPredictor::GlobalHistoryPredictor() { GHR_ = 0; }
bool GlobalHistoryPredictor::predict(uint32_t pc) {
  return PHT_[pc ^ GHR_].predict();
}
void GlobalHistoryPredictor::refresh(uint32_t pc, bool taken) {
  PHT_[pc ^ GHR_].refresh(taken);
  GHR_ = ((GHR_ << 1) | (taken)) & GHRCOVER;
}

bool TournamentPredictor::predict(uint32_t pc) {
  bool select = meta_predictor_.predict(pc);
  return select ? predictor1_.predict(pc) : predictor2_.predict(pc);
}

void TournamentPredictor::refresh(uint32_t pc, bool taken) {
  bool result1 = predictor1_.predict(pc);
  bool result2 = predictor2_.predict(pc);
  if ((result1 || result2) && !(result1 && result2)) {
    meta_predictor_.refresh(pc, (result1 && taken) || !(result1 || taken));
  }
  predictor1_.refresh(pc, taken);
  predictor2_.refresh(pc, taken);
}

BranchTargetBuffer::BranchTargetBuffer() {
  for (int i = 0; i < PHTSIZE; ++i) {
    target_[i] = 0;
  }
}

uint32_t BranchTargetBuffer::predict(uint32_t pc) { return target_[pc]; }

void BranchTargetBuffer::refresh(uint32_t pc, uint32_t addr) {
  target_[pc] = addr;
}
