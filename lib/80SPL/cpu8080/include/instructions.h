#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

struct cpu8080;


void instruction_add(struct cpu8080 *cpu, uint8_t reg_x);

void instruction_adc(struct cpu8080 *cpu, uint8_t reg_x);

void instruction_sub(struct cpu8080 *cpu, uint8_t reg_x);

void instruction_sbb(struct cpu8080 *cpu, uint8_t reg_x);

void instruction_ana(struct cpu8080 *cpu, uint8_t reg_x);

void instruction_xra(struct cpu8080 *cpu, uint8_t reg_x);

void instruction_ora(struct cpu8080 *cpu, uint8_t reg_x);

void instruction_cmp(struct cpu8080 *cpu, uint8_t reg_x);

#endif

