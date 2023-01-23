#include "../include/cpu.h"

static void unimplemented_opcode(Cpu8080 *cpu, uint8_t *opcode){
    fprintf(stderr, "Unimplemented Opcode: %02X\n", *opcode);
    fprintf(stderr, "Step: %d\n", cpu->step_count);
    exit(1);
}

uint16_t cpu_GetLIWord(Cpu8080 *cpu){
    uint16_t word = cpu->memory->memory[++cpu->PC];
    word ^= (uint16_t) cpu->memory->memory[++cpu->PC] << 8;
    return word;
}


void cpu_init(Cpu8080 *cpu, Memory *memory){
    //Clear cpu registers
    memset(cpu, 0, sizeof(*cpu));
    //memset(used_opcodes, 0xff, sizeof(used_opcodes)); //USED in DEBUG log
    cpu->step_count = 0; //should be cycle count
    cpu->memory = memory;
}

void cpu_fetch_execute(Cpu8080 *cpu){

    if(cpu->rst_pin && cpu->interrupt_enable){ //RST, byte
        cpu->rst_pin = 0;
        stack_push16(cpu, cpu->PC);
        cpu->PC = INTERRUPT_VECTOR_SIZE * cpu->interrupt_address;
    }

    uint8_t opcode = memory_read8(cpu->memory, cpu->PC);
//    printf("%#04x\n", opcode);
    cpu_exec_instruction(cpu, &opcode);

    /*
    if(cpu->step_count > 5869894){ //starting step for log
        insert_opcode(opcode);
    }
    if(cpu->step_count > 6010000 ){ //final step for log
        exit(1);
    }
    */
    cpu->PC ++;
    cpu->step_count++;
}

void cpu_exec_instruction(Cpu8080 *cpu, uint8_t *opcode){
    //CREATE A FUNCTION FOR EACH INSTRUCTION IN SEPARATE FILE instructions.c
    uint16_t reg_M_adr = read_reg_HL(cpu);
    uint8_t reg_M = cpu->memory->memory[reg_M_adr];
    //could be defined in config.h
    //reg_M = memory content pointed by HL
    switch(*opcode){
        case 0x00: //NOP
            break;
        case 0x01: //LXI B, word 
            {
            uint16_t word = cpu_GetLIWord(cpu);
            write_reg_BC(cpu,word);
            break;
            }
        case 0x02: //STAX B 
            {
            uint16_t adr = read_reg_BC(cpu);
            cpu->memory->memory[adr] = cpu->reg_A;
            break;
            }
        case 0x03: //INX B 
            {
            uint16_t word = read_reg_BC(cpu);
            word ++;
            write_reg_BC(cpu, word);
            break;
            }
        case 0x04: //INR B 
            test_flag_ac(&cpu->flags, cpu->reg_B, 0x01, 0);
            cpu->reg_B++;
            test_normal_flags(&cpu->flags, cpu->reg_B);
            break;
        case 0x05: //DCR B 
            test_flag_ac(&cpu->flags, cpu->reg_B, 0xff, 0); 
            cpu->reg_B--;
            test_normal_flags(&cpu->flags, cpu->reg_B);
            break;
        case 0x06: //MVI B, byte 
            cpu->reg_B = memory_read8(cpu->memory, ++cpu->PC);
            break;
        case 0x07: //RLC 
            {
            uint8_t bit7 = cpu->reg_A & 0b10000000;
            cpu->reg_A <<= 1;
            if(bit7){
                cpu->flags.cy = 1;
                cpu->reg_A |= 0b00000001;
            }else{
                cpu->flags.cy = 0;
                cpu->reg_A &= ~0b00000001;
            }
            break;
            }
        case 0x09: //DAD B 
            {
            uint16_t word1 = read_reg_HL(cpu);
            uint16_t word2 = read_reg_BC(cpu);
            test_carry_flag16(&cpu->flags, (uint32_t) word1 + word2);
            word1 += word2;
            write_reg_HL(cpu, word1);
            break;
            }
        case 0x0a: //LDAX B 
            {
            uint16_t adr = read_reg_BC(cpu);
            cpu->reg_A = cpu->memory->memory[adr];
            break;
            }
        case 0x0b: //DCX B 
            {
            uint16_t word = read_reg_BC(cpu);
            write_reg_BC(cpu, word - 1);
            break;
            }
        case 0x0c: //INR C 
            test_flag_ac(&cpu->flags, cpu->reg_C, 0x01, 0);
            cpu->reg_C++;
            test_normal_flags(&cpu->flags, cpu->reg_C);
            break;
        case 0x0d: //DCR C 
            test_flag_ac(&cpu->flags, cpu->reg_C, 0xff, 0);
            cpu->reg_C--;
            test_normal_flags(&cpu->flags, cpu->reg_C);
            break;
        case 0x0e: //MVI C, byte 
            cpu->reg_C = memory_read8(cpu->memory, ++cpu->PC);
            break;
        case 0x0f: //RRC 
            {
            uint8_t bit0 = cpu->reg_A & 0b00000001;
            cpu->reg_A >>= 1;
            if(bit0){
                cpu->flags.cy = 1;
                cpu->reg_A |= 0b10000000;
            }else{
                cpu->flags.cy = 0;
                cpu->reg_A &= ~0b10000000;
            }
            break;
            }
        case 0x11: //LXI D, word 
            {
            uint16_t word = cpu_GetLIWord(cpu);
            write_reg_DE(cpu, word);
            break;
            }
        case 0x12: //STAX D 
            {
            uint16_t adr = read_reg_DE(cpu);
            cpu->memory->memory[adr] = cpu->reg_A;
            break;
            }
        case 0x13: //INX D 
            {
            uint16_t word = read_reg_DE(cpu) + 1;
            write_reg_DE(cpu, word);
            break;
            }
        case 0x14: //INR D 
            test_flag_ac(&cpu->flags, cpu->reg_D, 0x01, 0);
            cpu->reg_D++;
            test_normal_flags(&cpu->flags, cpu->reg_D);
            break;
        case 0x15: //DCR D 
            test_flag_ac(&cpu->flags, cpu->reg_D, 0xff, 0);
            cpu->reg_D--;
            test_normal_flags(&cpu->flags, cpu->reg_D);
            break;
        case 0x16: //MVI D, byte 
            cpu->reg_D = memory_read8(cpu->memory, ++cpu->PC);
            break;
        case 0x17: //RAL
            {
            uint8_t bit7 = cpu->reg_A & 0b10000000;
            cpu->reg_A <<= 1;
            if(cpu->flags.cy){
                cpu->reg_A |= 0b00000001;
            }
            if(bit7){
                cpu->flags.cy = 1;
            }else{
                cpu->flags.cy = 0;
            }
            break;
            }
        case 0x19: //DAD D 
            {
            uint16_t word1 = read_reg_HL(cpu);
            uint16_t word2 = read_reg_DE(cpu);
            test_carry_flag16(&cpu->flags, (uint32_t) word1 + word2);
            word1 += word2;
            write_reg_HL(cpu, word1);
            break;
            }
        case 0x1a: //LDAX D 
            {
            uint16_t adr = read_reg_DE(cpu);
            cpu->reg_A = cpu->memory->memory[adr];
            break;
            }
        case 0x1b: //DCX D 
            {
            uint16_t word = read_reg_DE(cpu);
            write_reg_DE(cpu, word - 1);
            break;
            }
        case 0x1c: //INR E 
            test_flag_ac(&cpu->flags, cpu->reg_E, 0x01, 0);
            cpu->reg_E++;
            test_normal_flags(&cpu->flags, cpu->reg_E);
            break;
        case 0x1d: //DCR E 
            test_flag_ac(&cpu->flags, cpu->reg_E, 0xff, 0);
            cpu->reg_E--;
            test_normal_flags(&cpu->flags, cpu->reg_E);
            break;
        case 0x1e: //MVI E, byte 
            cpu->reg_E = memory_read8(cpu->memory, ++cpu->PC);
            break;
        case 0x1f: //RAR 
            {
            uint8_t bit0 = cpu->reg_A & 0b00000001;
            cpu->reg_A >>= 1;
            if(cpu->flags.cy){
                cpu->reg_A |= 0b10000000;
            }
            if(bit0){
                cpu->flags.cy = 1;
            }else{
                cpu->flags.cy = 0;
            }
            break;
            }
        case 0x21: //LXI H, word 
            {
            uint16_t word = cpu_GetLIWord(cpu);
            write_reg_HL(cpu, word);
            break;
            }
        case 0x22: //SHLD adr  
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            cpu->memory->memory[adr] = cpu->reg_L;
            cpu->memory->memory[adr+1] = cpu->reg_H;
            break;
            }
        case 0x23: //INX H 
            {
            uint16_t word = read_reg_HL(cpu) + 1;
            write_reg_HL(cpu, word);
            break;
            }
        case 0x24: //INR H 
            test_flag_ac(&cpu->flags, cpu->reg_H, 0x01, 0);
            cpu->reg_H++;
            test_normal_flags(&cpu->flags, cpu->reg_H);
            break;
        case 0x25: //DCR H 
            test_flag_ac(&cpu->flags, cpu->reg_H, 0xff, 0);
            cpu->reg_H--;
            test_normal_flags(&cpu->flags, cpu->reg_H);
            break;
        case 0x26: //MVI H, byte 
            cpu->reg_H = memory_read8(cpu->memory, ++cpu->PC);
            break;
        case 0x27: //DAA 
            {
            bool cy = cpu->flags.cy;
            uint8_t correction = 0;

            uint8_t ls_nibble = cpu->reg_A & 0x0f;
            if((ls_nibble > 0x09) || cpu->flags.ac){
                correction += 0x06;
            }

            uint8_t ms_nibble = cpu->reg_A & 0xf0;
            if((ms_nibble > 0x90) || cpu->flags.cy || (ms_nibble >= 0x90 && ls_nibble > 9)){
                correction += 0x60;
                cy = 1; //if no overflow, remais unchanged, not reset
            }
            
            instruction_add(cpu, correction);
            cpu->flags.cy = cy;
            break;
            }
        case 0x29: //DAD H 
            {
            uint16_t word1 = read_reg_HL(cpu);
            uint16_t word2 = read_reg_HL(cpu);
            test_carry_flag16(&cpu->flags, (uint32_t) word1 + word2);
            word1 += word2;
            write_reg_HL(cpu, word1);
            break;
            }
        case 0x2a: //LHLD adr 
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            cpu->reg_L = cpu->memory->memory[adr];
            cpu->reg_H = cpu->memory->memory[adr+1];
            break;
            }
        case 0x2b: //DCX H
            {
            uint16_t word = read_reg_HL(cpu);
            write_reg_HL(cpu, word - 1);
            break;
            }
        case 0x2c: //INR L
            test_flag_ac(&cpu->flags, cpu->reg_L, 0x01, 0);
            cpu->reg_L++;
            test_normal_flags(&cpu->flags, cpu->reg_L);
            break;
        case 0x2d: //DCR L
            test_flag_ac(&cpu->flags, cpu->reg_L, 0xff, 0);
            cpu->reg_L--;
            test_normal_flags(&cpu->flags, cpu->reg_L);
            break;
        case 0x2e: //MVI L, byte
            cpu->reg_L = memory_read8(cpu->memory, ++cpu->PC);
            break;
        case 0x2f: //CMA 
            cpu->reg_A = ~cpu->reg_A;
            break;
        case 0x31: //LXI SP, word 
            {
            uint16_t word = cpu_GetLIWord(cpu);
            cpu->SP = word;
            break;
            }
        case 0x32: //STA adr 
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            cpu->memory->memory[adr] = cpu->reg_A;
            break;
            }
        case 0x33: //INX SP
            cpu->SP ++;
            break;
        case 0x34: //INR M 
            test_flag_ac(&cpu->flags, reg_M, 0x01, 0);
            cpu->memory->memory[reg_M_adr]++;
            reg_M++;
            test_normal_flags(&cpu->flags, reg_M);
            break;
        case 0x35: //DCR M
            test_flag_ac(&cpu->flags, reg_M, 0xff, 0);
            cpu->memory->memory[reg_M_adr]--;
            reg_M--;
            test_normal_flags(&cpu->flags, reg_M);
            break;
        case 0x36: //MVI M,byte
            cpu->memory->memory[reg_M_adr] = memory_read8(cpu->memory, ++cpu->PC);
            break;
        case 0x37: //STC
            cpu->flags.cy = 1;
            break;
        case 0x39: //DAD SP
            {
            uint16_t word = read_reg_HL(cpu);
            test_carry_flag16(&cpu->flags, (uint32_t) word + cpu->SP);
            word += cpu->SP;
            write_reg_HL(cpu, word);
            break;
            }
        case 0x3a: //LDA adr 
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            cpu->reg_A = cpu->memory->memory[adr];
            break;
            }
        case 0x3b: //DCX SP 
            cpu->SP --;
            break;
        case 0x3c: //INR A
            test_flag_ac(&cpu->flags, cpu->reg_A, 0x01, 0);
            cpu->reg_A++;
            test_normal_flags(&cpu->flags, cpu->reg_A);
            break;
        case 0x3d: //DCR A
            test_flag_ac(&cpu->flags, cpu->reg_A, 0xff, 0);
            cpu->reg_A --;
            test_normal_flags(&cpu->flags, cpu->reg_A);
            break;
        case 0x3e: //MVI A,byte
            cpu->reg_A = memory_read8(cpu->memory, ++cpu->PC);
            break;
        case 0x3f: //CMC
            cpu->flags.cy = ~cpu->flags.cy;
            break;
        case 0x40: //MOV B,B 
            cpu->reg_B = cpu->reg_B;
            break;
        case 0x41: //MOV B,C
            cpu->reg_B = cpu->reg_C;
            break;
        case 0x42: //MOV B,D 
            cpu->reg_B = cpu->reg_D;
            break;
        case 0x43: //MOV B,E 
            cpu->reg_B = cpu->reg_E;
            break;
        case 0x44: //MOV B,H 
            cpu->reg_B = cpu->reg_H;
            break;
        case 0x45: //MOV B,L 
            cpu->reg_B = cpu->reg_L;
            break;
        case 0x46: //MOV B,M 
            cpu->reg_B = reg_M;
            break;
        case 0x47: //MOV B,A 
            cpu->reg_B = cpu->reg_A;
            break;
        case 0x48: //MOV C,B 
            cpu->reg_C = cpu->reg_B;
            break;
        case 0x49: //MOV C,C 
            cpu->reg_C = cpu->reg_C;
            break;
        case 0x4a: //MOV C,D 
            cpu->reg_C = cpu->reg_D;
            break;
        case 0x4b: //MOV C,E 
            cpu->reg_C = cpu->reg_E;
            break;
        case 0x4c: //MOV C,H 
            cpu->reg_C = cpu->reg_H;
            break;
        case 0x4d: //MOV C,L
            cpu->reg_C = cpu->reg_L;
            break;
        case 0x4e: //MOV C,M
            cpu->reg_C = reg_M;
            break;
        case 0x4f: //MOV C,A 
            cpu->reg_C = cpu->reg_A;
            break;
        case 0x50: //MOV D,B 
            cpu->reg_D = cpu->reg_B;
            break;
        case 0x51: //MOV D,C 
            cpu->reg_D = cpu->reg_C;
            break;
        case 0x52: //MOV D,D 
            cpu->reg_D = cpu->reg_D;
            break;
        case 0x53: //MOV D,E 
            cpu->reg_D = cpu->reg_E;
            break;
        case 0x54: //MOV D,H 
            cpu->reg_D = cpu->reg_H;
            break;
        case 0x55: //MOV D,L 
            cpu->reg_D = cpu->reg_L;
            break;
        case 0x56: //MOV D,M
            cpu->reg_D = reg_M;
            break;
        case 0x57: //MOV D,A 
            cpu->reg_D = cpu->reg_A;
            break;
        case 0x58: //MOV E,B 
            cpu->reg_E = cpu->reg_B;
            break;
        case 0x59: //MOV E,C 
            cpu->reg_E = cpu->reg_C;
            break;
        case 0x5a: //MOV E,D 
            cpu->reg_E = cpu->reg_D;
            break;
        case 0x5b: //MOV E,E 
            cpu->reg_E = cpu->reg_E;
            break;
        case 0x5c: //MOV E,H 
            cpu->reg_E = cpu->reg_H;
            break;
        case 0x5d: //MOV E,L 
            cpu->reg_E = cpu->reg_L;
            break;
        case 0x5e: //MOV E,M 
            cpu->reg_E = reg_M;
            break;
        case 0x5f: //MOV E,A 
            cpu->reg_E = cpu->reg_A;
            break;
        case 0x60: //MOV H,B 
            cpu->reg_H = cpu->reg_B;
            break;
        case 0x61: //MOV H,C
            cpu->reg_H = cpu->reg_C;
            break;
        case 0x62: //MOV H,D
            cpu->reg_H = cpu->reg_D;
            break;
        case 0x63: //MOV H,E
            cpu->reg_H = cpu->reg_E;
            break;
        case 0x64: //MOV H,H
            cpu->reg_H = cpu->reg_H;
            break;
        case 0x65: //MOV H,L
            cpu->reg_H = cpu->reg_L;
            break;
        case 0x66: //MOV H,M
            cpu->reg_H = reg_M;
            break;
        case 0x67: //MOV H,A
            cpu->reg_H = cpu->reg_A;
            break;
        case 0x68: //MOV L,B
            cpu->reg_L = cpu->reg_B;
            break;
        case 0x69: //MOV L,C 
            cpu->reg_L = cpu->reg_C;
            break;
        case 0x6a: //MOV L,D 
            cpu->reg_L = cpu->reg_D;
            break;
        case 0x6b: //MOV L,E 
            cpu->reg_L = cpu->reg_E;
            break;
        case 0x6c: //MOV L,H
            cpu->reg_L = cpu->reg_H;
            break;
        case 0x6d: //MOV L,L
            cpu->reg_L = cpu->reg_L;
            break;
        case 0x6e: //MOV L,M
            cpu->reg_L = reg_M;
            break;
        case 0x6f: //MOV L,A
            cpu->reg_L = cpu->reg_A;
            break;
        case 0x70: //MOV M,B
            cpu->memory->memory[reg_M_adr] = cpu->reg_B;
            break;
        case 0x71: //MOV M,C 
            cpu->memory->memory[reg_M_adr] = cpu->reg_C;
            break;
        case 0x72: //MOV M,D 
            cpu->memory->memory[reg_M_adr] = cpu->reg_D;
            break;
        case 0x73: //MOV M,E
            cpu->memory->memory[reg_M_adr] = cpu->reg_E;
            break;
        case 0x74: //MOV M,H
            cpu->memory->memory[reg_M_adr] = cpu->reg_H;
            break;
        case 0x75: //MOV M,L 
            cpu->memory->memory[reg_M_adr] = cpu->reg_L;
            break;
        case 0x77: //MOV M,A 
            cpu->memory->memory[reg_M_adr] = cpu->reg_A;
            break;
        case 0x78: //MOV A,B 
            cpu->reg_A = cpu->reg_B;
            break;
        case 0x79: //MOV A,C 
            cpu->reg_A = cpu->reg_C;
            break;
        case 0x7a: //MOV A,D 
            cpu->reg_A = cpu->reg_D;
            break;
        case 0x7b: //MOV A,E 
            cpu->reg_A = cpu->reg_E;
            break;
        case 0x7c: //MOV A,H
            cpu->reg_A = cpu->reg_H;
            break;
        case 0x7d: //MOV A,L
            cpu->reg_A = cpu->reg_L;
            break;
        case 0x7e: //MOV A,M
            cpu->reg_A = reg_M;
            break;
        case 0x7f: //MOV A,A
            cpu->reg_A = cpu->reg_A;
            break;
        case 0x80: //ADD B
            instruction_add(cpu, cpu->reg_B);
            break;
        case 0x81: //ADD C
            instruction_add(cpu, cpu->reg_C);
            break;
        case 0x82: //ADD D
            instruction_add(cpu, cpu->reg_D);
            break;
        case 0x83: //ADD E
            instruction_add(cpu, cpu->reg_E);
            break;
        case 0x84: //ADD H
            instruction_add(cpu, cpu->reg_H);
            break;
        case 0x85: //ADD L
            instruction_add(cpu, cpu->reg_L);
            break;
        case 0x86: //ADD M
            instruction_add(cpu, reg_M);
            break;
        case 0x87: //ADD A
            instruction_add(cpu, cpu->reg_A);
            break;
        case 0x88: //ADC B
            instruction_adc(cpu, cpu->reg_B);
            break;
        case 0x89: //ADC C
            instruction_adc(cpu, cpu->reg_C);
            break;
        case 0x8a: //ADC D
            instruction_adc(cpu, cpu->reg_D);
            break;
        case 0x8b: //ADC E
            instruction_adc(cpu, cpu->reg_E);
            break;
        case 0x8c: //ADC H
            instruction_adc(cpu, cpu->reg_H);
            break;
        case 0x8d: //ADC L
            instruction_adc(cpu, cpu->reg_L);
            break;
        case 0x8e: //ADC M
            instruction_adc(cpu, reg_M);
            break;
        case 0x8f: //ADC A
            instruction_adc(cpu, cpu->reg_A);
            break;
        case 0x90: //SUB B
            instruction_sub(cpu, cpu->reg_B);
            break;
        case 0x91: //SUB C
            instruction_sub(cpu, cpu->reg_C);
            break;
        case 0x92: //SUB D
            instruction_sub(cpu, cpu->reg_D);
            break;
        case 0x93: //SUB E
            instruction_sub(cpu, cpu->reg_E);
            break;
        case 0x94: //SUB H
            instruction_sub(cpu, cpu->reg_H);
            break;
        case 0x95: //SUB L
            instruction_sub(cpu, cpu->reg_L);
            break;
        case 0x96: //SUB M
            instruction_sub(cpu, reg_M);
            break;
        case 0x97: //SUB A
            instruction_sub(cpu, cpu->reg_A);
            break;
        case 0x98: //SBB B
            instruction_sbb(cpu, cpu->reg_B);
            break;
        case 0x99: //SBB C
            instruction_sbb(cpu, cpu->reg_C);
            break;
        case 0x9a: //SBB D
            instruction_sbb(cpu, cpu->reg_D);
            break;
        case 0x9b: //SBB E
            instruction_sbb(cpu, cpu->reg_E);
            break;
        case 0x9c: //SBB H
            instruction_sbb(cpu, cpu->reg_H);
            break;
        case 0x9d: //SBB L
            instruction_sbb(cpu, cpu->reg_L);
            break;
        case 0x9e: //SBB M
            instruction_sbb(cpu, reg_M);
            break;
        case 0x9f: //SBB A
            instruction_sbb(cpu, cpu->reg_A);
            break;
        case 0xa0: //ANA B
            instruction_ana(cpu, cpu->reg_B);
            break;
        case 0xa1: //ANA C
            instruction_ana(cpu, cpu->reg_C);
            break;
        case 0xa2: //ANA D
            instruction_ana(cpu, cpu->reg_D);
            break;
        case 0xa3: //ANA E
            instruction_ana(cpu, cpu->reg_E);
            break;
        case 0xa4: //ANA H
            instruction_ana(cpu, cpu->reg_H);
            break;
        case 0xa5: //ANA L
            instruction_ana(cpu, cpu->reg_L);
            break;
        case 0xa6: //ANA M
            instruction_ana(cpu, reg_M);
            break;
        case 0xa7: //ANA A
            instruction_ana(cpu, cpu->reg_A);
            break;
        case 0xa8: //XRA B
            instruction_xra(cpu, cpu->reg_B);
            break;
        case 0xa9: //XRA C
            instruction_xra(cpu, cpu->reg_C);
            break;
        case 0xaa: //XRA D
            instruction_xra(cpu, cpu->reg_D);
            break;
        case 0xab: //XRA E
            instruction_xra(cpu, cpu->reg_E);
            break;
        case 0xac: //XRA H
            instruction_xra(cpu, cpu->reg_H);
            break;
        case 0xad: //XRA L
            instruction_xra(cpu, cpu->reg_L);
            break;
        case 0xae: //XRA M
            instruction_xra(cpu, reg_M);
            break;
        case 0xaf: //XRA A
            instruction_xra(cpu, cpu->reg_A);
            break;
        case 0xb0: //ORA B
            instruction_ora(cpu, cpu->reg_B);
            break;
        case 0xb1: //ORA C
            instruction_ora(cpu, cpu->reg_C);
            break;
        case 0xb2: //ORA D
            instruction_ora(cpu, cpu->reg_D);
            break;
        case 0xb3: //ORA E
            instruction_ora(cpu, cpu->reg_E);
            break;
        case 0xb4: //ORA H
            instruction_ora(cpu, cpu->reg_H);
            break;
        case 0xb5: //ORA L
            instruction_ora(cpu, cpu->reg_L);
            break;
        case 0xb6: //ORA M
            instruction_ora(cpu, reg_M);
            break;
        case 0xb7: //ORA A
            instruction_ora(cpu, cpu->reg_A);
            break;
        case 0xb8: //CMP B
            instruction_cmp(cpu, cpu->reg_B);
            break;
        case 0xb9: //CMP C
            instruction_cmp(cpu, cpu->reg_C);
            break;
        case 0xba: //CMP D
            instruction_cmp(cpu, cpu->reg_D);
            break;
        case 0xbb: //CMP E
            instruction_cmp(cpu, cpu->reg_E);
            break;
        case 0xbc: //CMP H
            instruction_cmp(cpu, cpu->reg_H);
            break;
        case 0xbd: //CMP L
            instruction_cmp(cpu, cpu->reg_L);
            break;
        case 0xbe: //CMP M
            instruction_cmp(cpu, reg_M);
            break;
        case 0xbf: //CMP A
            instruction_cmp(cpu, cpu->reg_A);
            break;
        case 0xc0: //RNZ
            if(!cpu->flags.z){
                cpu->PC = stack_pop16(cpu) -1;
            }
            break;
        case 0xc1: //POP B
            {
            uint16_t word = stack_pop16(cpu);
            write_reg_BC(cpu, word);
            break;
            }
        case 0xc2: //JNZ adr
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            if(!cpu->flags.z){
                cpu->PC = adr -1;
            }
            break;
            }
        case 0xc3: //JMP adr
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            cpu->PC = adr -1;
            break;
            }
        case 0xc4: //CNZ adr
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            if(!cpu->flags.z){
                stack_push16(cpu, cpu->PC+1);
                cpu->PC = adr -1;
            }
            break;
            }
        case 0xc5: //PUSH B
            {
            uint16_t word = read_reg_BC(cpu);
            stack_push16(cpu, word);
            break;
            }
        case 0xc6: //ADI byte 
            {
            uint8_t operand = memory_read8(cpu->memory, ++cpu->PC);
            instruction_add(cpu, operand);
            break;
            }
        case 0xc7: //RST 0
            stack_push16(cpu, cpu->PC+1);
            cpu->PC = (INTERRUPT_VECTOR_SIZE * 0) - 1;
            break;
        case 0xc8: //RZ
            if(cpu->flags.z){
                cpu->PC = stack_pop16(cpu) -1;
            }
            break;
        case 0xc9: //RET
            cpu->PC = stack_pop16(cpu) -1;
            break;
        case 0xca: //JZ adr
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            if(cpu->flags.z){
                cpu->PC = adr -1;
            }
            break;
            }
        case 0xcc: //CZ
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            if(cpu->flags.z){
                stack_push16(cpu, cpu->PC+1);
                cpu->PC = adr -1;
            }
            break;
            }
        case 0xcd: //CALL adr
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            if(adr == 0x0000){
                printf("end of test\n");
                exit(1);
            }else if(adr == 0x0005){
                if(cpu->reg_C == 0x02){
                    printf("%c", cpu->reg_E);
                }
                if(cpu->reg_C == 0x09){
                    uint16_t adr = read_reg_DE(cpu);
                    char letter = cpu->memory->memory[adr];
                    while(letter != '$'){
                        letter = cpu->memory->memory[adr];
                        printf("%c", letter);
                        adr++;
                    }
                    //printf("\n");
                    /*
                    if(cpu->step_count > 10){
                        printf("STEP: %d", cpu->step_count - 6);
                        exit(1);
                    }
                    */
                }
            }else{
            stack_push16(cpu, cpu->PC+1);
            cpu->PC = adr -1;
            }
            break;
            }
        case 0xce: //ACI byte
            {
            uint8_t operand = memory_read8(cpu->memory, ++cpu->PC); 
            instruction_adc(cpu, operand);
            break;
            }
        case 0xcf: //RST 1
            stack_push16(cpu, cpu->PC+1);
            cpu->PC = (INTERRUPT_VECTOR_SIZE * 1) - 1;
            break;
        case 0xd0: //RNC
            if(!cpu->flags.cy){
                cpu->PC = stack_pop16(cpu) -1;
            }
            break;
        case 0xd1: //POP D
            {
            uint16_t word = stack_pop16(cpu);
            write_reg_DE(cpu, word);
            break;
            }
        case 0xd2: //JNC adr
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            if(!cpu->flags.cy){
                cpu->PC = adr -1;
            }
            break;
            }
        case 0xd3: //OUT byte 
            {
            //duplicated byte on address bus
            uint16_t adr = (uint16_t) memory_read8(cpu->memory, ++cpu->PC);
            adr |= adr << 8;
            cpu->address_bus = adr; 
            //reg_A on data_bus
            cpu->data_bus = cpu->reg_A;
            cpu->IO_M_pin = 1; 
            cpu->WR_pin = 0; //active when 0
            cpu->RD_pin = 1; //active when 0
            break;
            }
        case 0xd4: //CNC adr
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            if(!cpu->flags.cy){
                stack_push16(cpu, cpu->PC+1);
                cpu->PC = adr -1;
            }
            break;
            }
        case 0xd5: //PUSH D
            stack_push16(cpu, read_reg_DE(cpu));
            break;
        case 0xd6: //SUI byte 
            {
            uint8_t operand = memory_read8(cpu->memory, ++cpu->PC); 
            instruction_sub(cpu, operand);
            break;
            }
        case 0xd7: //RST 2;
            stack_push16(cpu, cpu->PC+1);
            cpu->PC = (INTERRUPT_VECTOR_SIZE * 2) - 1;
            break;
        case 0xd8: //RC
            if(cpu->flags.cy){
                cpu->PC = stack_pop16(cpu) -1;
            }
            break;
        case 0xda: //JC adr
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            if(cpu->flags.cy){
                cpu->PC = adr -1;
            }
            break;
            }
        case 0xdb: //IN byte
            {
            uint16_t adr = (uint16_t) memory_read8(cpu->memory, ++cpu->PC);
            adr |= adr << 8;
            cpu->address_bus = adr; 
            cpu->IO_M_pin = 1; 
            cpu->WR_pin = 1; //active when 0
            cpu->RD_pin = 0; //active when 0
            break;
            }
        case 0xdc: //CC adr
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            if(cpu->flags.cy){
                stack_push16(cpu, cpu->PC+1);
                cpu->PC = adr -1;
            }
            break;
            }
        case 0xde: //SBI byte 
            {
            uint8_t operand = memory_read8(cpu->memory, ++cpu->PC); 
            instruction_sbb(cpu, operand);
            break;
            }
        case 0xdf: //RST 3;
            stack_push16(cpu, cpu->PC+1);
            cpu->PC = (INTERRUPT_VECTOR_SIZE * 3) - 1;
            break;
        case 0xe0: //RPO
            if(!cpu->flags.p){
                cpu->PC = stack_pop16(cpu) -1;
            }
            break;
        case 0xe1: //POP H
            {
            uint16_t word = stack_pop16(cpu);
            write_reg_HL(cpu, word);
            break;
            }
        case 0xe2: //JPO adr
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            if(!cpu->flags.p){
                cpu->PC = adr -1;
            }
            break;
            }
        case 0xe3: //XTHL 
            {
            uint16_t word = stack_pop16(cpu);
            stack_push16(cpu, read_reg_HL(cpu));
            write_reg_HL(cpu, word);
            break;
            }
        case 0xe4: //CPO adr
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            if(!cpu->flags.p){
                stack_push16(cpu, cpu->PC+1);
                cpu->PC = adr -1;
            }
            break;
            }
        case 0xe5: //PUSH H
            stack_push16(cpu, read_reg_HL(cpu));
            break;
        case 0xe6: //ANI byte 
            {
            uint8_t operand = memory_read8(cpu->memory, ++cpu->PC); 
            instruction_ana(cpu, operand);
            break;
            }
        case 0xe7: //RST 4;
            stack_push16(cpu, cpu->PC+1);
            cpu->PC = (INTERRUPT_VECTOR_SIZE * 4) - 1;
            break;
        case 0xe8: //RPE
            if(cpu->flags.p){
                cpu->PC = stack_pop16(cpu) -1;
            }
            break;
        case 0xe9: //PCHL
            {
            uint16_t word = read_reg_HL(cpu);
            cpu->PC = word -1;
            break;
            }
        case 0xea: //JPE adr
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            if(cpu->flags.p){
                cpu->PC = adr -1;
            }
            break;
            }
        case 0xeb: //XCHG
            {
            uint16_t temp = read_reg_HL(cpu);
            write_reg_HL(cpu, read_reg_DE(cpu));
            write_reg_DE(cpu, temp);
            break;
            }
        case 0xec: //CPE adr
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            if(cpu->flags.p){
                stack_push16(cpu, cpu->PC+1);
                cpu->PC = adr -1;
            }
            break;
            }
        case 0xee: //XRI byte 
            {
            uint8_t operand = memory_read8(cpu->memory, ++cpu->PC); 
            instruction_xra(cpu, operand);
            break;
            }
        case 0xef: //RST 5;
            stack_push16(cpu, cpu->PC+1);
            cpu->PC = (INTERRUPT_VECTOR_SIZE * 5) - 1;
            break;
        case 0xf0: //RP
            if(!cpu->flags.s){
                cpu->PC = stack_pop16(cpu) -1;
            }
            break;
        case 0xf1: //POP PSW
            {
            uint8_t flags_reg = stack_pop8(cpu);
            flags_sta_byte(&cpu->flags, flags_reg);
            cpu->reg_A = stack_pop8(cpu);
            break;
            }
        case 0xf2: //JP
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            if(!cpu->flags.s){
                cpu->PC = adr -1;
            }
            break;
            }
        case 0xf3: //DI
            cpu->interrupt_enable = 0;
            break;
        case 0xf4: //CP adr
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            if(!cpu->flags.s){
                stack_push16(cpu, cpu->PC+1);
                cpu->PC = adr -1;
            }
            break;
            }
        case 0xf5: //PUSH PSW
            {
            uint8_t flags_reg = flags_load_byte(&cpu->flags);
            stack_push8(cpu, cpu->reg_A);
            stack_push8(cpu, flags_reg);
            break;
            }
        case 0xf6: //ORI byte 
            {
            uint8_t operand = memory_read8(cpu->memory, ++cpu->PC); 
            instruction_ora(cpu, operand);
            break;
            }
        case 0xf7: //RST 6
            stack_push16(cpu, cpu->PC+1);
            cpu->PC = (INTERRUPT_VECTOR_SIZE * 6) - 1;
            break;
        case 0xf8: //RM
            if(cpu->flags.s){
                cpu->PC = stack_pop16(cpu) -1;
            }
            break;
        case 0xf9: //SPHL
            {
            uint16_t word = read_reg_HL(cpu);
            cpu->SP = word;
            break;
            }
        case 0xfa: //JM adr
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            if(cpu->flags.s){
                cpu->PC = adr -1;
            }
            break;
            }
        case 0xfb: //EI
            cpu->interrupt_enable = 1;
            break;
        case 0xfc: //CM adr
            {
            uint16_t adr = cpu_GetLIWord(cpu);
            if(cpu->flags.s){
                stack_push16(cpu, cpu->PC+1);
                cpu->PC = adr -1;
            }
            break;
            }
        case 0xfe: //CPI byte 
            {
            uint8_t operand = memory_read8(cpu->memory, ++cpu->PC);
            instruction_cmp(cpu, operand);
            break;
            }
        case 0xff: //RST 7
            stack_push16(cpu, cpu->PC + 1);
            cpu->PC = (INTERRUPT_VECTOR_SIZE * 7) - 1;
            break;
        default:
            unimplemented_opcode(cpu,opcode);
            break;
    }
}


