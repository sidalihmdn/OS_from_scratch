#include "keyboard.h"
#include "../io_functions/low_level.h"
#include "../drivers/screen.h"
#include "../int/pic.h"

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

void init_keyboard(void) {
    // Clear keyboard buffer
    while (inb(KEYBOARD_STATUS_PORT) & 0x01) {
        inb(KEYBOARD_DATA_PORT);
    }
    
    // Enable keyboard interrupts (unmask IRQ1)
    unsigned char mask = inb(PIC1_DATA);
    mask &= ~(1 << 1); // Clear bit 1 (IRQ1)
    outb(PIC1_DATA, mask);
}

extern "C" void keyboard_handler(void) {
    unsigned char scancode = inb(KEYBOARD_DATA_PORT);
    
    // Only handle key press (bit 7 = 0 means pressed)
    if (!(scancode & 0x80)) {
        char ascii = scancode_to_ascii[scancode];
        
        if (ascii != 0) {
            // Print the character
            char str[2] = {ascii, '\0'};
            print_string(str, 1);
        }
    }
    
    // Send End of Interrupt to PIC
    PIC_sendEOI(1);
}
