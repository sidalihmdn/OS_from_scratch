#include "../../includes/drivers/display/console.h"
#include "../../includes/boot/multiboot_helpers.h"
#include "../../includes/cpu/int.h"
#include "../../includes/unit_types.h"
#include "../../includes/drivers/display/vbe.h"
#include "../../includes/drivers/display/font.h"

static uint32_t cursor_x;
static uint32_t cursor_y;
static uint32_t cursor_color;
static uint32_t bg_color;
static uint32_t screen_usable_height;
static uint32_t screen_usable_width;

void console_scroll();

void console_init(multiboot_info_t* mb_info){
    vbe_init(mb_info);
    cursor_x = CONSOLE_MARGIN;
    cursor_y = CONSOLE_MARGIN;
    cursor_color = vbe_rgb(255, 255, 255);
    bg_color = vbe_rgb(0, 0, 0);
    screen_usable_height = vbe_get_height() - CONSOLE_MARGIN;
    screen_usable_width = vbe_get_width() - CONSOLE_MARGIN;
}

void console_put_char(char c){
    disable_interrupts();
    if (c >= 32 && c <= 126){
        font_draw_char(cursor_x, cursor_y, cursor_color, c);
        cursor_x+=8;
    }

    /*
    * special caracters handling
    */
    if (c == '\n'){
        cursor_x = CONSOLE_MARGIN;
        cursor_y+=8;
    }
    if (c == '\t'){
        cursor_x+=32;
    }
    if (c == '\b'){
        if (cursor_x > CONSOLE_MARGIN){
            cursor_x-=8;
            font_erase_char(cursor_x, cursor_y, bg_color);
        }
    }

    if (cursor_x >= screen_usable_width){
        cursor_x = CONSOLE_MARGIN;
        cursor_y+=8;
    }
    if (cursor_y >= screen_usable_height){
        console_scroll();
    }
    enable_interrupts();
}

void console_scroll(){
    vbe_scroll_up(1, bg_color);
    cursor_y -= 8;
}