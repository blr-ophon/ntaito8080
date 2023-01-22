
/*
 * TODO:
 * 1)memory outside of cpu //
 * 2)I/O instructions, data and address buffers //
 * 3)interrupt structure inside cpu + priority //
 * 4)interrupt vectors and IACK // IACK not necessary
 *      8080 only has 1 RST pin
 *      8085 has 5
 * 5)external full shift register in I/O port 
 * 6)display VRAM and implement hardware interrupts at 60hz
 */

void machine_cycle(){
    //opt.: send memoryPtr to CPU
    //
}

/*
 *
 * main(){
 *      create a memory structure
 *      init_memory(rom, adress): clear memory and load ROM
 *      create a cpu structure
 *      init_cpu(cpu, memory): clear registers and link cpu to a memory
 *      
 *      clear interrupts
 *      while(program_running){
 *          send interrupts
 *          execute cpu 
 *
 *          read address/data buffers
 *          (*)cpu will clear buffers at the start of each exec
 *              //this means 0x0000 cant be used as a port address
 *          if(buffers){
 *             w/r = cpu.wr 
 *             write or read in respective port
 *          }
 *
 *          process_externals(
 *              draw screen
 *              set interrupts
 *          )
 *      }
 *      
 *
 * }
 */
