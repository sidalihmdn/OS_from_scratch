#include "../drivers/screen.h"
#include "../includes/string.h"
#include "../int/int.h"
#include "../int/pic.h"
#include "../drivers/keyboard.h"

#if DEBUG
//some debug code
#endif

void main(){
    clean_screen();
    print_string((char *)"os > ", 5);
    //print_string(int2String(1234),4);
    set_idt();
    int a = 12/0;
    PIC_remap(32,40);
    init_keyboard();
    for(;;){}
    //print_char(0, -1 , -1 , '0'+ len((char *)"les"));
    return;

}
