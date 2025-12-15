#ifndef ARCH_TIME_H
#define ARCH_TIME_H

#include <unit_types.h>

// Hardware frequency constant (arch-specific, provided by implementation)
extern const uint32_t ARCH_TIMER_BASE_HZ;

/**
 * Initialize the architecture-specific timer hardware
 * @param hz Desired timer frequency in Hz
 * @return 0 on success, -1 on error
 */
int arch_timer_init(uint32_t hz);

/**
 * Get the current hardware tick count (if available)
 * @return Current tick count from hardware counter
 */
uint64_t arch_timer_get_ticks(void);

/**
 * Send End of Interrupt for timer
 */
void arch_timer_eoi(void);

/**
 * Enable timer interrupts
 */
void arch_timer_enable(void);

/**
 * Disable timer interrupts
 */
void arch_timer_disable(void);

/**
 * Get capabilities of this timer implementation
 */
typedef struct {
    uint32_t max_frequency;      // Maximum supported frequency
    uint32_t min_frequency;      // Minimum supported frequency
    bool supports_one_shot;      // Can do one-shot timers
    bool has_high_res_counter;   // Has high-resolution counter (TSC, etc.)
} arch_timer_caps_t;

void arch_timer_get_caps(arch_timer_caps_t* caps);

#endif