#ifndef RTC_H
#define RTC_H

#include <unit_types.h>
struct rtc_time_t{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
};

void arch_rtc_init();
void arch_rtc_get_time(rtc_time_t* time);
void arch_rtc_set_time(rtc_time_t* time);
bool arch_rtc_is_updating();


#endif