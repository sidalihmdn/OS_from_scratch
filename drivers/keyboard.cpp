#include "keyboard.h"
#include "../io_functions/low_level.h"
#include "../includes/unit_types.h"
#include "../int/int.h"
#include "../int/pic.h"
#include "screen.h"
#include "../includes/scancode_map_tables.h"
#include "../includes/string.h"


// declarations
uint8_t scancode();
void entry_process(uint8_t scancode);  

buffer string_buffer;
// the interrupt handler function
__attribute__((interrupt)) void keyboard_callback(idt_frame *frame){
  uint8_t c = scancode();
  entry_process(c); 
  send_EOI(1);
}

// initialize the irq 
void init_keyboard(){
  init_idt_desc((void *)keyboard_callback,
                INT_GATE_FLAG,
                33);

}

uint8_t scancode(){
  uint8_t scancode = inb(0x60);
  return scancode;
}

// clear the buffer
void clear_buffer(buffer *buf){
  for (int i = 0; i < buf->len ; i++){
    buf->data[i] = 0x00;  
  }
  buf->len = 0;
}

char* get_buffer(){
  return &string_buffer.data[0];
}

void entry_process(uint8_t scancode){
  if (scancode >= 0 && scancode < 128 &&  kbd_US[scancode] != 0){
      char character = kbd_US[scancode];
      if (character == '\n'){
        
        string_buffer.data[string_buffer.len] = '\0';
        print_char(0,-1,-1,'\n'); // TODO: add a jump to the next line function that prints the os :> thing
        zprint(string_buffer.data);
        clear_buffer(&string_buffer);
  
      }
      else{
        string_buffer.data[string_buffer.len++] = kbd_US[scancode];
        print_char(0,-1,-1,kbd_US[scancode]);
      }
     
    }
}
