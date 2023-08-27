#include "time.h"
#include "../drivers/screen.h"
#include "../io_functions/low_level.h"
#include "string.h"
#include "unit_types.h"

// declarations
uint8_t read_rtc(uint8_t reg);
void set_binary24hours_mode(void);

void get_date_time(date_time *date_struct) {
  set_binary24hours_mode();
  date_struct->year = read_rtc(RTC_YEAR);
  date_struct->month = read_rtc(RTC_MONTH);
  date_struct->day = read_rtc(RTC_DAY);
  date_struct->hour = read_rtc(RTC_HOURS);
  date_struct->minute = read_rtc(RTC_MINUTES);
  date_struct->second = read_rtc(RTC_SECONDS);
}

// this will return the value stored inside the register
// (month , days,  etc)
uint8_t read_rtc(uint8_t reg) {
  // the NMI bit is the highest order bit
  // it's needs to be set every time we try to access a register
  outb(0x70, (1 << 7) | reg); // this why we set the NMI bit
  return inb(0x71);
}

// this will set the rtc on binary mode
// originaly it's on bcd mode
void set_binary24hours_mode() {
  outb(0x70, (1 << 7) | REG_B);
  uint8_t data = inb(REG_B);
  outb(0x70, (1 << 7) | REG_B);
  outb(0x71, (data | 0x06));
}

void print_date() {
  static date_time date;
  get_date_time(&date);
  char *date_string[3] = {int2String(date.year), int2String(date.month),
                          int2String(date.day)};

  char *date_ = strCatMulti(date_string, 3);
  // print_string(strCatMulti(date_string, 5), 60, 1);
  // print_string(int2String(date.year), 60, 2);
  zprint(date_);
}
