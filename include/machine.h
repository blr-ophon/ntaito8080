#ifndef MACHINE_H
#define MACHINE_H
#include "./cpu.h"

typedef struct machine{
    Cpu8080 cpu;
    uint8_t IO_ports[7];
}Machine;

#endif


    /*TODO: remove memory from CPU
     * so that the cpu can be ported
     * to other machines
     * 1) instead of writing directly to
     * memory in instructions, should 
     * instead write to buffers and set signals
     * memory enable and R/W like real CPU,
     * that will be seen by memory and than reset
     * for next instruction
     * *HOWEVER: This means I wont be able to write
     * and read 16 bit words directly unless I use a 
     * 16bit data buffer buffercpu
     *
     * The quick and not really bad solution:
     * create memory outside of cpu, and than import it
     * int (receiving a size value in init) and exporting
     * changes made in cpu memory to real memory at 
     * the end of each cycle in case any other device
     * needs memory for things like DMA
     *
     * The quick, simple and not bad solution:
     * make cpu_receive a pointer to an external
     * memory so that it can modify it and handle
     * memory related errors
    */


 /*      //ALL cpu-memory interactions are handled inside cpu
 *
 *      I/O: 
 *      1) in IN and OUT, the 8085 will put the
 *      port number (in hex), duplicated in the 
 *      adress bus, alongside a W/R signal. the
 *      machine should handle what to do with
 *      these
 *      
 *      2)
 *      8085:
 *      before each exec, the cpu will check
 *      for interrupts and handle them in priority
 *      order. After going to interrupt vector, it
 *      will set the IACK. The machine should 
 *      know which interrupt to stop after recei-
 *      ving the IACK.
 *
 *      8080:
 *      external output sets RST pin and provides
 *      a number between 0 and 7 to data latch.
 *      Each RST vector contains 8 bytes
 *
 *
 *      ********FOR 8080**************
 *      INTERRUPT:  PRIORITY:   VECTOR:
 *      TRAP        1           0x0024
 *      RST 7.5     2           0x003C
 *      RST 6.5     3           0x0034
 *      RST 5.5     4           0x003C
 *      INTR        5             -
 *
 *
 *      ********FOR 8085**************
 *      INTERRUPT:  PRIORITY:   VECTOR:
 *      TRAP        1           0x0024
 *      RST 7.5     2           0x003C
 *      RST 6.5     3           0x0034
 *      RST 5.5     4           0x003C
 *      INTR        5             -

        
    VRAM to screen:
    **60hz, 2 interrupts; half and end screen
*
* I/O Ports:
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
