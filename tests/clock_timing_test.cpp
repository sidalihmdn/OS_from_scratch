#include <drivers/clock.h>
#include <drivers/display/console.h>
#include <libc/string.h>

void test_pit_accuracy() {
    printk("\n=== PIT Timing Accuracy Test ===\n");
    
    // Test 1: Count ticks over known RTC interval
    printk("\n[Test 1] Measuring PIT ticks over RTC seconds\n");
    
    rtc_time_t start_rtc, end_rtc;
    uint64_t start_ticks, end_ticks;
    
    // Wait for RTC second boundary
    clock_get_date_time(&start_rtc);
    uint8_t start_sec = start_rtc.second;
    while (start_rtc.second == start_sec) {
        clock_get_date_time(&start_rtc);
    }
    
    // Now we're at the boundary of a new second
    clock_get_time(&start_ticks);
    uint8_t ref_sec = start_rtc.second;
    
    // Wait for 10 full RTC seconds
    do {
        clock_get_date_time(&end_rtc);
    } while ((end_rtc.second < ref_sec + 10) && (end_rtc.second >= ref_sec || end_rtc.second < 10));
    
    clock_get_time(&end_ticks);
    
    uint64_t elapsed_ticks = end_ticks - start_ticks;
    uint32_t rtc_seconds = 10;
    
    printk("RTC elapsed: %d seconds\n", rtc_seconds);
    printk("PIT ticks: %d\n", (uint32_t)elapsed_ticks);
    printk("Expected ticks: %d (for 1000 Hz)\n", rtc_seconds * 1000);
    printk("Actual frequency: %d Hz\n", (uint32_t)(elapsed_ticks / rtc_seconds));
    
    // Test 2: Sleep test
    printk("\n[Test 2] Sleep accuracy test\n");
    uint64_t sleep_start, sleep_end;
    
    clock_get_time(&sleep_start);
    clock_sleep_s(5);
    clock_get_time(&sleep_end);
    
    uint64_t sleep_ticks = sleep_end - sleep_start;
    printk("Requested: 5 seconds\n");
    printk("PIT ticks: %d\n", (uint32_t)sleep_ticks);
    printk("Expected: 5000 ticks\n");
    printk("Error: %d ticks (%d ms)\n", (uint32_t)(sleep_ticks - 5000), (uint32_t)(sleep_ticks - 5000));
    
    printk("\n=== Test Complete ===\n\n");
}
