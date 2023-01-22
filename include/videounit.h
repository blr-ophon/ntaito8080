#ifndef VIDEOUNIT_H
#define VIDEOUNIT_H
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "./config.h"
#include "../lib/80SPL/80SPL.h"

typedef struct videounit{
    SDL_Window *window;
    SDL_Renderer *renderer;
    unsigned int width;
    unsigned int height;
    unsigned int window_multiplier;
    bool RST_pin;
    unsigned int current_scanline;
}VideoUnit;

void videounit_init(VideoUnit *videounit);

void videounit_stop(VideoUnit *videounit);

void videounit_clearframe(VideoUnit *videounit);

void videounit_Renderscanline(VideoUnit *videounit, Memory *memory);



#endif
