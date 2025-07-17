#ifndef GLOBAL_HPP
#define GLOBAL_HPP
#include "storage.hpp"
/*Acturally, every stage register is a group of registers, however, as we
describe the whole control sys in a behavior way, we can ignore most of them
only symbolizing the data use to calculate or be calculated.*/

extern StageRegister IF_ID_reg_1; // for instruction
extern StageRegister IF_ID_reg_2; // for PC count
extern StageRegister IF_ID_reg_3; // for jump or not

extern StageRegister ID_EX_reg_1; // for register1
extern StageRegister ID_EX_reg_2; // for register2

extern StageRegister EX_MEM_reg_1; // for ALU
extern StageRegister EX_MEM_reg_2; // for zeroflag--showing possible branch miss
extern StageRegister EX_MEM_reg_3; // for register2

#endif