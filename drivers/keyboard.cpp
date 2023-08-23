#include "keyboard.h"
#include "../includes/unit_types.h"
#include "../int/int.h"
#include "../int/pic.h"

void keyboard_callback(){
  print_string((char *)"hehe",4);
}

void init_keyboard(){
  init_idt_desc((void *)keyboard_callback,
                INT_GATE_FLAG,
                IRQ1);

}
