#include "../includes/drivers/keyboard.h"
#include "../includes/cpu/ports.h"
#include "../includes/drivers/screen.h"
#include "../includes/cpu/pic.h"
#include "../includes/drivers/display/font.h"
#include "../includes/drivers/display/vbe.h"
#include "../includes/drivers/display/console.h"
// Simple US keyboard layout (scancode set 1)
const char scancode_to_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, // Control
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, // Left shift
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0, // Right shift
    '*',
    0, // Alt
    ' ', // Space
    0, // Caps lock
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F1-F10
    0, // Num lock
    0, // Scroll lock
    0, 0, 0, // Home, Up, Page Up
    '-',
    0, 0, 0, // Left, Center, Right
    '+',
    0, 0, 0, 0, // End, Down, Page Down, Insert
    0, // Delete
    0, 0, 0,
    0, 0, // F11, F12
    0, // All other keys undefined
};

static char key_buffer[256];
static int buffer_pos = 0;
static bool line_ready = false;

void init_keyboard(void) {
    // Clear keyboard buffer
    while (inb(KEYBOARD_STATUS_PORT) & 0x01) {
        inb(KEYBOARD_DATA_PORT);
    }
    
    // Enable keyboard interrupts (unmask IRQ1)
    unsigned char mask = inb(PIC1_DATA);
    mask &= ~(1 << 1); // Clear bit 1 (IRQ1)
    outb(PIC1_DATA, mask);
    
    buffer_pos = 0;
    line_ready = false;
}

bool get_input_buffer(char* buffer, int max_size) {
    if (!line_ready) return false;
    
    int i;
    for (i = 0; i < buffer_pos && i < max_size - 1; i++) {
        buffer[i] = key_buffer[i];
    }
    buffer[i] = 0;
    
    buffer_pos = 0;
    line_ready = false;
    return true;
}

extern "C" void keyboard_handler(void) {
    unsigned char scancode = inb(KEYBOARD_DATA_PORT);
    
    // Only handle key press (bit 7 = 0 means pressed)
    if (!(scancode & 0x80)) {
        char ascii = scancode_to_ascii[scancode];
        
        if (ascii == '\n') { // Enter
            console_put_char('\n');
            line_ready = true;
        } else if (ascii == '\b') { // Backspace
            if (buffer_pos > 0) {
                buffer_pos--;
                console_put_char('\b'); // Move cursor back
            }
        } else if (ascii != 0) {
            if (buffer_pos < 255 && !line_ready) {
                key_buffer[buffer_pos++] = ascii;
                console_put_char(ascii);
            }
        }
    }
    vbe_swap_buffer();
    
    // Send End of Interrupt to PIC
    PIC_sendEOI(1);
}
