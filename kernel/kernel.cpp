#include "../drivers/screen.h"
#include "../includes/string.h"
#include "../int/int.h"
#include "../int/pic.h"
#include "../drivers/keyboard.h"
#include "../io_functions/low_level.h"
#if DEBUG
//some debug code
#endif

void main(){
    clean_screen();
    zprint("os > ");
    int data = inb(0x61);     
    outb(0x61,data | 0x80);//Disables the keyboard  
    outb(0x61,data & 0x7F);//Enables the keyboard  
    PIC_remap(0x20,0x28);
    IRQ_clear_mask(1);
    //print_string(int2String(1234),4);
    set_idt();
    int a = 12/0;
    print_string((char *)"hrhr\n",5);
    //int b = 12/0;
    
    for(;;){}
    //print_char(0, -1 , -1 , '0'+ len((char *)"les"));
    return;

}
