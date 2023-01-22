#ifndef CONFIG_H
#define CONFIG_H
//separate machine config from cpu config

#define SIZE_OF_MEMORY 0x4400
#define VRAM_START_ADRESS 0x2400

#define INTERRUPT_FREQUENCY 120
#define INTERRUPT_PERIOD 1000/INTERRUPT_FREQUENCY

#define ORIGINAL_WIDTH 256
#define ORIGINAL_HEIGHT 224
#define ORIGINAL_RATIO 7/8
#define PIXEL_SIZE 1

#endif
