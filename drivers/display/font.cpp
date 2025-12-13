#include "../../includes/drivers/display/font.h"
#include "../../includes/drivers/display/font8x8_basic.h"
#include "../../includes/unit_types.h"
#include "../../includes/drivers/display/vbe.h"

void font_draw_char(uint32_t x, uint32_t y, uint32_t color, uint8_t c){
    for(uint32_t i = 0; i < 8; i++){
        for(uint32_t j = 0; j < 8; j++){
            if(font8x8_basic[c][i] & (1 << j)){
                vbe_put_pixel(x + j, y + i, color);
            }
        }
    }
}

void font_erase_char(uint32_t x, uint32_t y, uint32_t bg_color){
    for(uint32_t i = 0; i < 8; i++){
        for(uint32_t j = 0; j < 8; j++){
            vbe_put_pixel(x + j, y + i, bg_color);
        }
    }
} 