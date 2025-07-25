#ifndef DECODER_HPP
#define DECODER_HPP
#include "utils.hpp"

class Decoder {
private:
  Decoder() = default;

public:
  static Decoder &getInstance();
  DecodeInsInfo parse(BasicInsInfo ins, BranchPredictInfo branch);
};

#endif