#include "../drivers/screen.h"
#define VIDEO_ADDRESS 0xb8000

#if DEBUG
//some debug code
#endif

extern "C" void main(){
    print_char(0, 1,1 , 'A');
    return;
}