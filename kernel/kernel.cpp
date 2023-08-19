#include "../drivers/screen.h"

#if DEBUG
//some debug code
#endif

void main(){
    clean_screen();
    print_string("os > ", 5);
    return;

}