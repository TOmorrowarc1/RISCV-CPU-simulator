#include "run.hpp"
/*
Wires keep the work inside them, while functions keep the control logic,
quiring information from the InsBoard to arrange the dataflow.
*/
void StageIF();
void StageID();
void StageEXE();
void StageMEM();
void StageWB();