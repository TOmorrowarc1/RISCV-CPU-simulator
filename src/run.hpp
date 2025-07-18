#ifndef RUN_HPP
#define RUN_HPP

#include "calculate.hpp"
#include "memory.hpp"
/*
Wires keep the work inside them, while functions keep the control logic,
quiring information from the InsBoard to arrange the dataflow.
*/
void StageIF();
void StageID();
void StageEXE();
void StageMEM();
void StageWB();
void refreshStage();

#endif