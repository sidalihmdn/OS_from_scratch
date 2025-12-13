#include <arch/rtc.h>
#include <cpu/ports.h>

#define CMOS_PORT_ADDR 0x70
#define CMOS_DATA 0x71

#define RTC_REG_SEC 0x00
#define RTC_REG_MIN 0x02
#define RTC_REG_HR 0x04
#define RTC_REG_DAY 0x07
#define RTC_REG_MONTH 0x08
#define RTC_REG_YEAR 0x09

/*
 * Status Register A : for checking the update in progress bit
 * Status Register B : for formatting the data
 */
#define RTC_STATUS_A 0x0A
#define RTC_STATUS_B 0x0B

/*
 * UIP (Update In Progress)
 */
#define RTC_UIP_FLAG        0x80 

#define BCD_TO_BINARY(bcd) ((((bcd) >> 4) * 10) + ((bcd) & 0x0F))

uint8_t rtc_read_register(uint8_t reg){
    outb(CMOS_PORT_ADDR, reg);
    return inb(CMOS_DATA);
}

uint8_t rtc_write_register(uint8_t reg, uint8_t value){
    outb(CMOS_PORT_ADDR, reg);
    outb(CMOS_DATA, value);
    return value;
}

bool arch_rtc_is_updating(){
    return rtc_read_register(RTC_STATUS_A) & RTC_UIP_FLAG;
}
void arch_rtc_get_time(rtc_time_t* time){
    while (arch_rtc_is_updating());
    time->second = rtc_read_register(RTC_REG_SEC);
    time->minute = rtc_read_register(RTC_REG_MIN);
    time->hour = rtc_read_register(RTC_REG_HR);
    time->day = rtc_read_register(RTC_REG_DAY);
    time->month = rtc_read_register(RTC_REG_MONTH);
    time->year = rtc_read_register(RTC_REG_YEAR);
    
    /*
     * If the status register B has the DSE (Decimal Separator Enable) bit set,
     * the data is in BCD format, otherwise it is in binary format
     */
    uint8_t status_b = rtc_read_register(RTC_STATUS_B);
    if (status_b & 0x04){
        time->second = BCD_TO_BINARY(time->second);
        time->minute = BCD_TO_BINARY(time->minute);
        time->hour = BCD_TO_BINARY(time->hour);
        time->day = BCD_TO_BINARY(time->day);
        time->month = BCD_TO_BINARY(time->month);
        time->year = BCD_TO_BINARY(time->year);
    }
}

void arch_rtc_set_time(rtc_time_t* time){
    while (arch_rtc_is_updating());
    rtc_write_register(RTC_REG_SEC, time->second);
    rtc_write_register(RTC_REG_MIN, time->minute);
    rtc_write_register(RTC_REG_HR, time->hour);
    rtc_write_register(RTC_REG_DAY, time->day);
    rtc_write_register(RTC_REG_MONTH, time->month);
    rtc_write_register(RTC_REG_YEAR, time->year);
}
void arch_rtc_init(){
    rtc_write_register(RTC_STATUS_B, 0x04);   
}

