#pragma once

#define RTC_SECONDS 0x00
#define RTC_MINUTES 0x02
#define RTC_HOURS 0x04
#define RTC_DAY 0x07
#define RTC_MONTH 0x08
#define RTC_YEAR 0x09
#define REG_A 0x0A
#define REG_B 0x0B

#include "../includes/unit_types.h"

struct date_time {
  uint8_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} __attribute__((packed));

void get_date_time(date_time *date);
void print_date();
