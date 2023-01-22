#ifndef GAMEINPUTS_H
#define GAMEINPUTS_H
#include <SDL2/SDL.h>

typedef struct gameinputs{
    uint8_t port0;
    uint8_t port1;
    uint8_t port2;
}GameInputs;

void gameinputs_process(SDL_Event event, GameInputs *gameinputs);

void gameinputs_init(GameInputs *gameinputs);

#endif
