#include "../include/main.h"

void process_input(Machine *machine){
	SDL_Event event;
	SDL_PollEvent(&event);
    gameinputs_process(event, &machine->gameinputs);
	switch(event.type){
		case SDL_QUIT:		//Event of clicking X button to close
            machine->machine_enable = false;
			break;
		case SDL_KEYDOWN:
			if(event.key.keysym.sym == SDLK_ESCAPE){
                machine->machine_enable = false;
			}
            break;
	}
}

void videounit_interrupt(Cpu8080 *cpu, VideoUnit *videounit){
    cpu->rst_pin = 1;
    //draw scanlines starting from current (0 to 86) || (87 to 224)
    if(videounit->current_scanline == 97){
        for(int i = 97; i < 224; i++){ 
            videounit_Renderscanline(videounit, cpu->memory);
            videounit->current_scanline ++;
        }
        SDL_RenderPresent(videounit->renderer);
        cpu->interrupt_address = 2;
    }

    if(videounit->current_scanline == 0){
        for(int i = 0; i < 97; i++){ 
            videounit_Renderscanline(videounit, cpu->memory);
            videounit->current_scanline ++;
        }
//        SDL_RenderPresent(videounit->renderer);
        cpu->interrupt_address = 1;
    }

    if(videounit->current_scanline >= 224){
        videounit->current_scanline = 0;
    }
}

int last_interrupt_time;
//TODO: add this to videounit

/*
bool interrupt_delay(){
    int time_passed = SDL_GetTicks() - last_interrupt_time;
    if(time_passed <= INTERRUPT_FREQUENCY){
        return false;
    }
    last_interrupt_time = SDL_GetTicks();
    return true;
}
*/

void machine_loop(Machine *machine){
//    If I understand this right then the system gets RST 8 when the beam 
//    is *near* the middle of the screen and RST 10 when it is at the end (start of VBLANK).
        //check if 8ms(120hz) have passed
    //if(interrupt_delay()){
    if(machine->cpu.step_count % 4166 == 0){ //500.000hz / 120hz = 4166 inst/inter
        process_input(machine);
        videounit_interrupt(&machine->cpu, &machine->videounit);
    }
}

int main(int argc, char *argv[]){
    const char *filename;
    if(argc < 2){
        fprintf(stderr, "Must provide a file\n");
        exit(EXIT_FAILURE);
    }else{
        filename = argv[1];
        fprintf(stdin, " Loading file: %s\n", filename);
    }
    FILE *f = fopen(filename, "rb");
    if(!f){
        fprintf(stderr, "Failed to read from file\n");
        exit(EXIT_FAILURE);
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t buf[size];
    int res = fread(buf, 1, size, f);
    if(!res){
        fprintf(stderr, "Failed to read from file\n");
        exit(EXIT_FAILURE);
    }

    Machine machine;
    machine.machine_enable = true;

    Memory *memory = memory_init(0x10000);
    memory_clear(memory);
    memory_program_load(memory, buf, size,  0x0000);
    //memory_dump(memory, size);
    fclose(f);

    cpu_init(&machine.cpu, memory);
    gameinputs_init(&machine.gameinputs);
    videounit_init(&machine.videounit);

    machine.shift_reg_offset = 0;
    //CREATE init_machine function
    SDL_Init(SDL_INIT_EVERYTHING);


    while(machine.machine_enable){
        machine_loop(&machine);

        cpu_fetch_execute(&machine.cpu);
        if(machine.cpu.IO_M_pin){
            machine.cpu.IO_M_pin = 0; //reset IO pin
            uint8_t io_adr = machine.cpu.address_bus & 0x00ff;
            io_adr |= machine.cpu.RD_pin << 3; //WRITE: 1xxx, READ: 0xxx 
            switch(io_adr){
                case 0x00: //0000: input port 0 (0000)
                    machine.cpu.reg_A = machine.gameinputs.port0;
                    break;
                case 0x01: //0001: input port 1 (0001)
                    machine.cpu.reg_A = machine.gameinputs.port1;
                    break;
                case 0x02: //0010: input port 2 (0010)
                    machine.cpu.reg_A = machine.gameinputs.port2;
                    break;
                case 0x03: //0011: read from shift register with offset
                    {
                    uint16_t result = machine.shift_register;
                    result <<= machine.shift_reg_offset;
                    result >>= 8;
                    machine.cpu.reg_A = (uint8_t) result;
                    break;
                    }
                case 0x0a: //1010: write 02H
                    //set shift register offset
                    {
                    machine.shift_reg_offset = machine.cpu.data_bus;
                    break;
                    }
                case 0x0b: //1011: write 03H
                    //sound bits
                    break;
                case 0x0c: //1100: write 04H
                    machine.shift_register >>= 8;
                    machine.shift_register |= ((uint16_t) machine.cpu.data_bus << 8);
                    break;
                case 0x0d: //1101 sounds bits
                    break;
                case 0x0e: //1110 watchdog
                    break;
            }
        }
    }
    memory_free(memory);
    videounit_stop(&machine.videounit);
    return 0;
}

        //read data buffers for I/O
        /* I/O Ports:
            Read
            00        INPUTS (Mapped in hardware but never used by the code) //0000
            01        INPUTS                                                 //0001
            02        INPUTS                                                 //0010
            03        bit shift register read                                //0011
            Write
            02        shift amount (3 bits)                                  //1010 (10)
            03        sound bits                                             //1011 (11)
            04        shift data                                             //1100 (12)
            05        sound bits                                             //1101 (13)
            06        watch-dog                                              //1110 (14)
        */
