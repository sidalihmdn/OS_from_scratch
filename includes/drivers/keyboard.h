#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEYBOARD_DATA_PORT      0x60
#define KEYBOARD_STATUS_PORT    0x64
#define KEYBOARD_COMMAND_PORT   0x64

/// @brief Initialize the keyboard driver
void init_keyboard(void);

/// @brief Keyboard interrupt handler
extern "C" void keyboard_handler(void);

// Returns true if a full line (Enter pressed) is available in the buffer
// Copies the line to the provided buffer and clears the internal buffer
bool get_input_buffer(char* buffer, int max_size);

#endif // KEYBOARD_H
