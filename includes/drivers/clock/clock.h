#ifndef CLOCK_H
#define CLOCK_H

#include <unit_types.h>
#include <arch/rtc.h>

typedef void (*timer_callback_t)(void);

void init_clock();
void clock_set_handler(timer_callback_t handler);
void clock_get_time(uint64_t* timer);
void clock_get_up_time(uint64_t* timer);

void clock_sleep_ms(uint32_t ms);
void clock_sleep_s(uint32_t s);

void clock_get_date_time(rtc_time_t* time);
void clock_set_date_time(rtc_time_t* time);

typedef int timer_id_t;
timer_id_t clock_create_timer(uint32_t ms, timer_callback_t callback);
void clock_destroy_timer(timer_id_t timer_id);

uint32_t clock_get_frequency();

#endif