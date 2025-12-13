#include <drivers/clock/clock.h>
#include <arch/time.h>
#include <arch/rtc.h>
#include <cpu/int.h>

static volatile uint64_t g_ticks = 0;
static volatile uint64_t g_seconds = 0;
static volatile uint32_t g_frequency = 0;

static uint32_t tick_divider = 0;

static timer_callback_t g_user_callback = NULL;
#define MAX_TIMERS 10

typedef struct timer_event_t {
    uint64_t delay;
    bool active;
    timer_callback_t callback;
};

static struct timer_event_t g_timers[MAX_TIMERS];



static void generic_timer_handler(registers_t){
    g_ticks++;
    tick_divider++;
    if (g_frequency == tick_divider){
        g_seconds++;
        tick_divider = 0;
    }
    if (g_user_callback != NULL){
        g_user_callback();
    }
    /*
     * because the frequency is 1000, we can use g_ticks as ms
     * and we don't need to divide g_ticks by g_frequency 
     */ 
    uint64_t current_ms = g_ticks; 
    for (int i = 0; i < MAX_TIMERS; i++){
        if (g_timers[i].active && current_ms >= g_timers[i].delay){
            g_timers[i].active = false;
            g_timers[i].callback();
        }
    }
    arch_timer_eoi();
}

void init_clock(){
    g_frequency = 1000;
    register_interrupt_handler(32, generic_timer_handler);
    
    if (arch_timer_init(g_frequency) != 0){
        return;
    }
    arch_timer_enable();
}

void clock_set_handler(timer_callback_t handler){
    g_user_callback = handler;
}

void clock_get_time(uint64_t* time_ms){
    if (time_ms){
        *time_ms = g_ticks;
    }
}
    
void clock_get_up_time(uint64_t* time_s){
    if (time_s){
        *time_s = g_seconds;
    }
}

 timer_id_t clock_create_timer(uint32_t ms, timer_callback_t callback){
    uint64_t current_ms;
    clock_get_time(&current_ms);
    for (int i = 0; i < MAX_TIMERS; i++){
        if (!g_timers[i].active){
            g_timers[i].delay = current_ms + ms;
            g_timers[i].active = true;
            g_timers[i].callback = callback;
            return i;
        }
    }
    return -1; 
}

void clock_destroy_timer(timer_id_t timer_id){
    if (timer_id >= 0 && timer_id < MAX_TIMERS){
        g_timers[timer_id].active = false;
    }
}

void clock_sleep_ms(uint32_t ms){
    uint64_t current_ms;
    uint64_t start_ms;
    clock_get_time(&start_ms);
    do{
        clock_get_time(&current_ms);
    } while (current_ms < start_ms + ms);
}

void clock_sleep_s(uint32_t s){
    clock_sleep_ms(s * 1000);
}

uint32_t clock_get_frequency(){
    return g_frequency;
}

void clock_get_date_time(rtc_time_t* time){
    arch_rtc_get_time(time);
}

void clock_set_date_time(rtc_time_t* time){
    arch_rtc_set_time(time);
}

    