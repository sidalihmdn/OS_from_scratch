#include "../drivers/screen.h"

#if DEBUG
//some debug code
#endif

void main(){
    clean_screen();
    print_string("hello world!", 12);
    return;

}