#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "../lib/80SPL/80SPL.h"
#include "./videounit.h"
#include "./gameinputs.h"
#include "./config.h"


typedef struct machine{
    bool machine_enable;        //handles on/off
    Cpu8080 cpu;                //handles logical processing
    uint16_t shift_register;    
    uint8_t shift_reg_offset;
    VideoUnit videounit;        //handles video processing
    GameInputs gameinputs;
}Machine;

bool interrupt_delay();

#endif
