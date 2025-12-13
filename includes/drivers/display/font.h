#ifndef FONT_H
#define FONT_H

#include "../../unit_types.h"

void font_draw_char(uint32_t x, uint32_t y, uint32_t color, uint8_t c);
void font_erase_char(uint32_t x, uint32_t y, uint32_t bg_color);

#endif