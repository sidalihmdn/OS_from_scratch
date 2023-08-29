#include "keyboard.h"
#include "../includes/string.h"
#include "../includes/unit_types.h"
#include "../int/int.h"
#include "../int/pic.h"
#include "../io_functions/low_level.h"
#include "screen.h"

// declarations
__attribute__((no_caller_saved_registers)) uint8_t scancode();
__attribute__((no_caller_saved_registers)) void entry_process(uint8_t scancode);

buffer string_buffer;
// the interrupt handler function
//
//
// table for azert keyboard

const char kbd_US[128] = {
    0,   27,   '1', '2',  '3',  '4',  '5',  '6', '7',
    '8', '9',  '0', '-',  '=',  '\b', '\t', /* <-- Tab */
    'q', 'w',  'e', 'r',  't',  'y',  'u',  'i', 'o',
    'p', '[',  ']', '\n', 0, /* <-- control key */
    'a', 's',  'd', 'f',  'g',  'h',  'j',  'k', 'l',
    ';', '\'', '`', 0,    '\\', 'z',  'x',  'c', 'v',
    'b', 'n',  'm', ',',  '.',  '/',  0,    '*', 0, /* Alt */
    ' ',                                            /* Space bar */
    0,                                              /* Caps lock */
    0,                                              /* 59 - F1 key ... > */
    0,   0,    0,   0,    0,    0,    0,    0,   0, /* < ... F10 */
    0,                                              /* 69 - Num lock*/
    0,                                              /* Scroll Lock */
    0,                                              /* Home key */
    0,                                              /* Up Arrow */
    0,                                              /* Page Up */
    '-', 0,                                         /* Left Arrow */
    0,   0,                                         /* Right Arrow */
    '+', 0,                                         /* 79 - End key*/
    0,                                              /* Page Down */
    0,                                              /* Insert Key */
    0,                                              /* Delete Key */
    0,   0,    0,   0,                              /* F11 Key */
    0,                                              /* F12 Key */
    0, /* All other keys are undefined */
};

__attribute__((interrupt)) void keyboard_callback(idt_frame *frame) {
  entry_process(scancode());
  send_EOI(1);
}

// initialize the irq
void init_keyboard() {
  IRQ_clear_mask(1);
  init_idt_desc((void *)keyboard_callback, INT_GATE_FLAG, 33);
}

uint8_t scancode() {
  uint8_t scancode = inb(0x60);
  return scancode;
}

// clear the buffer
void clear_buffer(buffer *buf) {
  for (int i = 0; i < buf->len; i++) {
    buf->data[i] = 0x00;
  }
  buf->len = 0;
}

char *get_buffer() { return &string_buffer.data[0]; }

void entry_process(uint8_t scancode) {
  if (scancode >= 0 && scancode < 128 && kbd_US[scancode] != 0) {
    char character = kbd_US[scancode];
    if (character == '\n') {

      string_buffer.data[string_buffer.len] = '\0';
      print_char(0, -1, -1, '\n'); // TODO: add a jump to the next line function
                                   // that prints the os :> thing
      zprint(string_buffer.data);
      clear_buffer(&string_buffer);

    } else {
      string_buffer.data[string_buffer.len++] = kbd_US[scancode];
      print_char(0, -1, -1, kbd_US[scancode]);
    }
  }
}
