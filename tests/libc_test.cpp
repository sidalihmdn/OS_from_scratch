#include <tests/libc_test.h>
#include <libc/string.h>
#include <kernel/mem/pmm.h>
#include <kernel/panic.h>
#include <drivers/screen.h>

void test_string() {
    printk("\nTesting String Lib...\n");
    // Test len
    if (len("hello") == 5) {
        printk("  len: OK\n");
    } else {
        printk("  len: FAIL\n");
    }
    
    // Test strcmp
    if (strcmp("abc", "abc") == 0 && strcmp("abc", "abd") != 0) {
        printk("  strcmp: OK\n");
    } else {
        printk("  strcmp: FAIL\n");
    }
    
    // Test int2String
    // Note: int2String returns a static buffer, so we copy it to check
    char* s = int2String(123);
    if (strcmp(s, "123") == 0) {
        printk("  int2String: OK\n");
    } else {
        printk("  int2String: FAIL\n");
    }
}

void test_memory() {
    printk("\nTesting PMM...\n");
    uint32_t page1 = pmm_alloc_page();
    uint32_t page2 = pmm_alloc_page();
    pmm_free_page(page1);
    uint32_t page3 = pmm_alloc_page();
    if(page1 == page3){
        printk("  PMM: OK\n");
    } else {
        printk("  PMM: FAIL\n");
    }
}

void run_libc_tests() {
    print((char*)"--- Running LibC Tests ---\n");
    test_string();
    test_memory();
    print((char*)"--- Tests Finished ---\n");
}
