#include "../include/gameinputs.h"

/*
Port 0
    bit 0 DIP4 (Seems to be self-test-request read at power up)
    bit 1 Always 1
    bit 2 Always 1
    bit 3 Always 1
    bit 4 Fire
    bit 5 Left
    bit 6 Right
    bit 7 ? tied to demux port 7 ?

Port 1
    bit 0 = CREDIT (1 if deposit)
    bit 1 = 2P start (1 if pressed)
    bit 2 = 1P start (1 if pressed)
    bit 3 = Always 1
    bit 4 = 1P shot (1 if pressed)
    bit 5 = 1P left (1 if pressed)
    bit 6 = 1P right (1 if pressed)
    bit 7 = Not connected

Port 2
    **ships = lives
    bit 0 = DIP3 00 = 3 ships  10 = 5 ships
    bit 1 = DIP5 01 = 4 ships  11 = 6 ships
    bit 2 = Tilt
    bit 3 = DIP6 0 = extra ship at 1500, 1 = extra ship at 1000 
    bit 4 = P2 shot (1 if pressed)
    bit 5 = P2 left (1 if pressed)
    bit 6 = P2 right (1 if pressed)
    bit 7 = DIP7 Coin info displayed in demo screen 0=ON

Port 3
    bit 0-7 Shift register data
*/

void gameinputs_init(GameInputs *gameinputs){
    gameinputs->port0 = 0b11111111; 
    gameinputs->port1 = 0b00001001; 
    gameinputs->port2 = 0b00000011;
}

void gameinputs_process(SDL_Event event, GameInputs *gameinputs){
	switch(event.type){
		case SDL_KEYDOWN:
            //coins
			if(event.key.keysym.sym == SDLK_RETURN){
                gameinputs->port1 &= ~0b00000001; 
			}
            //2P start
			if(event.key.keysym.sym == SDLK_2){
                gameinputs->port1 |= 0b00000010;
			}
            //1P start
			if(event.key.keysym.sym == SDLK_1){
                gameinputs->port1 |= 0b00000100;
			}
            //1P shoot
			if(event.key.keysym.sym == SDLK_SPACE){
                gameinputs->port1 |= 0b00010000;
			}
            //1P left 
			if(event.key.keysym.sym == SDLK_LEFT){
                gameinputs->port1 |= 0b00100000;
			}
            //1P right
			if(event.key.keysym.sym == SDLK_RIGHT){
                gameinputs->port1 |= 0b01000000;
			}
            break;
		case SDL_KEYUP:
            //coins
			if(event.key.keysym.sym == SDLK_RETURN){
                gameinputs->port1 |= 0b00000001; 
			}
            //2P start
			if(event.key.keysym.sym == SDLK_2){
                gameinputs->port1 &= ~0b00000010;
			}
            //1P start
			if(event.key.keysym.sym == SDLK_1){
                gameinputs->port1 &= ~0b00000100;
			}
            //1P shoot
			if(event.key.keysym.sym == SDLK_SPACE){
                gameinputs->port1 &= ~0b00010000;
			}
            //1P left 
			if(event.key.keysym.sym == SDLK_LEFT){
                gameinputs->port1 &= ~0b00100000;
			}
            //1P right
			if(event.key.keysym.sym == SDLK_RIGHT){
                gameinputs->port1 &= ~0b01000000;
			}
            break;
	}
}
