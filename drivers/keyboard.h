#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEYBOARD_H
#define IRQ1 33

struct buffer{
  int len=0;
  char *data;
};


void init_keyboard(void);
char* get_buffer();
void clear_buffer(buffer *buf);
#endif
