#ifndef CONSOLE_H
#define CONSOLE_H

#include "../../unit_types.h"
#include "../../boot/multiboot_helpers.h"

#define CONSOLE_MARGIN 10

void console_init(multiboot_info_t* mb_info);
void console_put_char(char c);

#endif