#include "../drivers/screen.h"
#include "../includes/string.h"

#if DEBUG
//some debug code
#endif

void main(){
    clean_screen();
    print_string("os > ", 5);
    print_string(int2String(1234),4);
    //print_char(0, -1 , -1 , '0'+ len((char *)"les"));
    return;

}