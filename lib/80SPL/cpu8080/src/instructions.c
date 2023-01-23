#include "../include/instructions.h"
#include "../include/cpu.h"


void instruction_add(struct cpu8080 *cpu, uint8_t reg_x){
    uint16_t result = cpu->reg_A + reg_x;
    test_flag_ac(&cpu->flags, cpu->reg_A, reg_x, 0);
    test_carry_flag8(&cpu->flags, result);

    cpu->reg_A = result;
    test_normal_flags(&cpu->flags, cpu->reg_A);
}

void instruction_adc(struct cpu8080 *cpu, uint8_t reg_x){
    uint16_t result = cpu->reg_A + reg_x + cpu->flags.cy;
    test_flag_ac(&cpu->flags, cpu->reg_A, reg_x, cpu->flags.cy);
    test_carry_flag8(&cpu->flags, result);

    cpu->reg_A = result;
    test_normal_flags(&cpu->flags, cpu->reg_A);
}

void instruction_sub(struct cpu8080 *cpu, uint8_t reg_x){
    uint16_t result = cpu->reg_A + ~(reg_x) + 1;

    cpu->flags.cy = 0;
    if((result ^ cpu->reg_A ^ ~reg_x) & 0x0100){ 
        cpu->flags.cy = 1;
    }

    test_flag_ac(&cpu->flags, cpu->reg_A, ~reg_x, 1);
    test_normal_flags(&cpu->flags, result);
    cpu->reg_A = result;
    cpu->flags.cy = ~(cpu->flags.cy) & 0x01;
}

void instruction_sbb(struct cpu8080 *cpu, uint8_t reg_x){
    uint8_t borrow = ~(cpu->flags.cy) & 0x01;
    uint16_t result = cpu->reg_A + ~reg_x + borrow;

    cpu->flags.cy = 0;
    if((result ^ cpu->reg_A ^ ~reg_x) & 0x0100){ 
        cpu->flags.cy = 1;
    }

    test_flag_ac(&cpu->flags, cpu->reg_A, ~reg_x, borrow);
    test_normal_flags(&cpu->flags, result);
    cpu->reg_A = result;
    cpu->flags.cy = ~cpu->flags.cy & 0x01;
}

void instruction_cmp(struct cpu8080 *cpu, uint8_t reg_x){
    uint16_t result = cpu->reg_A - reg_x;
    cpu->flags.cy = result >> 8;
    cpu->flags.ac = 0;
    if(~(cpu->reg_A ^ result ^ reg_x) & 0x10){
        cpu->flags.ac = 1;
    }
    test_normal_flags(&cpu->flags, (uint8_t) result);
}

void instruction_ana(struct cpu8080 *cpu, uint8_t reg_x){
    uint8_t result = cpu->reg_A & reg_x;
    cpu->flags.cy = 0;
    cpu->flags.ac = 0;
    if(((cpu->reg_A | reg_x) & 0x08) != 0){
        cpu->flags.ac = 1;
    }
    test_normal_flags(&cpu->flags, result);
    cpu->reg_A = result;
}

void instruction_xra(struct cpu8080 *cpu, uint8_t reg_x){
    cpu->reg_A ^= reg_x;
    cpu->flags.ac = 0;
    cpu->flags.cy = 0;
    test_normal_flags(&cpu->flags, cpu->reg_A);
}

void instruction_ora(struct cpu8080 *cpu, uint8_t reg_x){
    cpu->reg_A |= reg_x;
    cpu->flags.ac = 0;
    cpu->flags.cy = 0;
    test_normal_flags(&cpu->flags, cpu->reg_A);
}



