#ifndef CPU_H
#define CPU_H 

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "./stack.h"
#include "./memory.h"
#include "./flags.h"
#include "./registerbank.h"
#include "./instructions.h"
#include "./config.h"

typedef struct cpu8080{
    uint8_t reg_A;
    uint8_t reg_B;
    uint8_t reg_C;
    uint8_t reg_D;
    uint8_t reg_E;
    uint8_t reg_H;
    uint8_t reg_L;

    uint16_t PC;
    uint16_t SP;

    uint8_t data_bus;
    uint16_t address_bus;

    Memory *memory;
    Flags flags;

    bool RD_pin:1;    //active when 0
    bool WR_pin:1;    //active when 0
    bool IO_M_pin:1;  //memory when 0, IO when 1
    bool rst_pin:1;
    bool interrupt_enable:1;
    uint8_t interrupt_address;
    uint32_t step_count;
} Cpu8080;

uint16_t cpu_GetLIWord(Cpu8080 *cpu);

void cpu_init(Cpu8080 *cpu, Memory *memory);

void cpu_fetch_execute(Cpu8080 *cpu);

void cpu_exec_instruction(Cpu8080 *cpu, uint8_t *opcode);

#endif
