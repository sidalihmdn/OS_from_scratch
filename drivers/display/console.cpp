#include "../../includes/drivers/display/console.h"
#include "../../includes/boot/multiboot_helpers.h"
#include "../../includes/unit_types.h"
#include "../../includes/drivers/display/vbe.h"
#include "../../includes/drivers/display/font.h"

static uint32_t cursor_x;
static uint32_t cursor_y;
static uint32_t cursor_color;
static uint32_t screen_height;
static uint32_t screen_width;

void console_init(multiboot_info_t* mb_info){
    vbe_init(mb_info);
    cursor_x = 0;
    cursor_y = 0;
    cursor_color = vbe_rgb(255, 255, 255);
    screen_height = vbe_get_height();
    screen_width = vbe_get_width();
}

void console_put_char(char c){
    draw_char(cursor_x, cursor_y, cursor_color, c);
    cursor_x+=8;
    if (cursor_x >= screen_width){
        cursor_x = 0;
        cursor_y+=8;
    }
    if (cursor_y >= screen_height){
        cursor_y = 0;
    }
}
    