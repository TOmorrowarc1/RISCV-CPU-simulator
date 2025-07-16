#ifndef GLOBAL_HPP
#define GLOBAL_HPP
#include "storage.hpp"

extern StageRegister IF_ID_reg;    // for instruction
extern StageRegister ID_EX_reg_1;  // for register1
extern StageRegister ID_EX_reg_2;  // for register2
extern StageRegister ID_EX_reg_3;  // for immdiate
extern StageRegister EX_MEM_reg_1; // for ALU
extern StageRegister EX_MEM_reg_2; // for zeroflag
extern StageRegister EX_MEM_reg_3; // for register2

#endif