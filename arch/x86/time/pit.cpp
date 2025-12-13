#include <cpu/ports.h>
#include <unit_types.h>
#include <cpu/int.h>
#include <cpu/pic.h>
#include <arch/time.h>

#define PIT_CHANNEL_0_DATA_PORT 0x40
#define PIT_COMMAND_PORT 0x43
#define PIT_BASE_FREQ 1193182

const uint32_t ARCH_TIMER_BASE_HZ = PIT_BASE_FREQ;

int arch_timer_init(uint32_t hz){
    if(hz == 0 || hz > 596591){
        return -1;
    }

    uint32_t divisor = PIT_BASE_FREQ / hz;

    /*
    * 0x36 is the command for:
    * - 0x36 hex = 00110110 binary
    * - Set the timer to square wave mode , access mode = lobyte/hibyte , channel = 0
    */
    outb(PIT_COMMAND_PORT, 0x36);
    outb(PIT_CHANNEL_0_DATA_PORT, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL_0_DATA_PORT, (uint8_t)(divisor >> 8));

    return 0;
}

void arch_timer_eoi(void){
    outb(0x20, 0x20);
}

void arch_timer_enable(void){
    PIC_clear_mask(0);
}
 
void arch_timer_disable(void){
    PIC_set_mask(0);
}

uint64_t arch_timer_get_ticks(void){
    /*
    * PIC does not provide a way to read the timer value
    * so we return 0
    */
    return 0;
}

void arch_timer_get_caps(arch_timer_caps_t* caps){
    caps->max_frequency = 596591;
    caps->min_frequency = 18;
    caps->supports_one_shot = false;
    caps->has_high_res_counter = false;
}

    