#include "control.hpp"
#include "init.hpp"
#include <iostream>


const int MEMSIZE = 10000;
uint32_t pc = 0;
uint32_t registers[32] = {0};
uint32_t memory[MEMSIZE] = {0};
