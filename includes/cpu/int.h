// this file contains the function declarations for interrup handling
// still learning about it
#ifndef INT_H
#define INT_H

#include <unit_types.h>
#include <drivers/screen.h>

typedef struct {
    uint32_t ds;                                     // Data segment selector
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
    uint32_t int_no, err_code;                       // Interrupt number and error code (if applicable)
    uint32_t eip, cs, eflags, useresp, ss;           // Pushed by the processor automatically.
} registers_t;

// Typedef for interrupt handler
typedef void (*isr_t)(registers_t);

/// @brief this function will register an interrupt handler
/// @param n the interrupt number
/// @param handler the handler function
void register_interrupt_handler(uint8_t n, isr_t handler);

/// @brief this function will initialise the exceptions
void init_exceptions();



#endif // !INT_H
