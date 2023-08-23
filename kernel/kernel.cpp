#include "../drivers/screen.h"
#include "../includes/string.h"
#include "../int/int.h"
#include "../drivers/keyboard.h"

#if DEBUG
//some debug code
#endif

void main(){
    clean_screen();
    print_string((char *)"os > ", 5);
    //print_string(int2String(1234),4);
    set_idt(); 
    //init_keyboard();
    //print_char(0, -1 , -1 , '0'+ len((char *)"les"));
    return;

}
