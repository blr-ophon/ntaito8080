#include "../include/videounit.h"

void videounit_init(VideoUnit *videounit){
    SDL_Init(SDL_INIT_EVERYTHING);
    //TODO:get adjusted width/multiplier
    //should do some math to see if screen will be
    //based on monitor width or height
    //ratio is 8:7
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    videounit->height = DM.h;
//    videounit->height = 256*2;
    videounit->width = (videounit->height*7)/8;
    videounit->window_multiplier = videounit->height/ORIGINAL_HEIGHT ;
    videounit->RST_pin = 0;
    videounit->current_scanline = 0;

    videounit->window = SDL_CreateWindow(
        "InvadersEmu",
        0,
        0,
        videounit->width,
        videounit->height,
        SDL_WINDOW_BORDERLESS
    );
    if(!videounit->window){
        //TODO
    }
    videounit->renderer = SDL_CreateRenderer(
        videounit->window,
        -1,
        0
    );
    if(!videounit->renderer){
        //TODO
    }
    videounit_clearframe(videounit);
}


void videounit_stop(VideoUnit *videounit){
    SDL_DestroyRenderer(videounit->renderer);
    SDL_DestroyWindow(videounit->window);
    SDL_Quit();
}

void videounit_clearframe(VideoUnit *videounit){
    SDL_SetRenderDrawColor(videounit->renderer, 0, 0, 0, 255);     //r,g,b,opacity
    SDL_RenderClear(videounit->renderer); 
    SDL_RenderPresent(videounit->renderer);
}


void videounit_Renderscanline(VideoUnit *videounit, Memory *memory){
    //nao renderizar pixels pretos
    //precisa renderizar de baixo para cima (2400 a 241f)
    //32 bytes de 8 bits = 256bits
    //
    //depois esquerda para direita incrementando 0x0020(32)
    //
    //e esquerda para a direita
    //
    //first_byteadr = 0x2400 + current_scanline*0x0020
    uint16_t adr = VRAM_START_ADRESS + videounit->current_scanline*0x20;
    int window_multiplier = videounit->window_multiplier;

    for(int i = 0; i < 32; i++){
        uint8_t byte = memory->memory[adr + i];
        for(int j = 0; j < 8; j++){
            uint8_t bit = (byte >> j) & 0b00000001;
            int x = videounit->current_scanline * window_multiplier;
            int y = (256 - (8*i + j)) *window_multiplier;
            if(bit){
                SDL_SetRenderDrawColor(videounit->renderer, 179, 255, 255, 255);     //light blue
                                                                                     
                if( y >= 35*window_multiplier && y <= 60*window_multiplier){
                    SDL_SetRenderDrawColor(videounit->renderer, 204, 153, 255, 255);     //purple
                }
                if( y >= 180*window_multiplier && y <= 240*window_multiplier){
                    SDL_SetRenderDrawColor(videounit->renderer, 153, 255, 153, 255);     //green
                }
            }else{
                SDL_SetRenderDrawColor(videounit->renderer, 0, 0, 0, 255);     //black
            }
            //color overlays on screen
            SDL_Rect pixel = {
                x,
                y,
                PIXEL_SIZE * window_multiplier,
                PIXEL_SIZE * window_multiplier
            };
            SDL_RenderFillRect(videounit->renderer, &pixel);
        }
    }
}

