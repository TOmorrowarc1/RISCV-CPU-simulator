#ifndef RUN_HPP
#define RUN_HPP
#include "CDBselector.hpp"
#include "Decoder.hpp"
#include "Executor.hpp"
#include "PC.hpp"
#include "ROB.hpp"
#include "RS.hpp"
#include "RegFile.hpp"

void StageFetch();
void StageIssue();
void StageExecute();
void StageBoardcast();
void StageCommit();

void RefreshStage();

void print_log();

#endif