#include "libc_test.h"
#include "../includes/libc/string.h"
#include "../includes/libc/mem.h"
#include "../includes/drivers/screen.h"

void test_string() {
    print((char*)"\nTesting String Lib...\n");
    
    // Test len
    if (len((char*)"hello") == 5) {
        print((char*)"  len: OK\n");
    } else {
        print((char*)"  len: FAIL\n");
    }
    
    // Test strcmp
    if (strcmp((char*)"abc", (char*)"abc") == 0 && strcmp((char*)"abc", (char*)"abd") != 0) {
        print((char*)"  strcmp: OK\n");
    } else {
        print((char*)"  strcmp: FAIL\n");
    }
    
    // Test int2String
    // Note: int2String returns a static buffer, so we copy it to check
    char* s = int2String(123);
    if (strcmp(s, (char*)"123") == 0) {
        print((char*)"  int2String: OK\n");
    } else {
        print((char*)"  int2String: FAIL\n");
    }
}

void test_memory() {
    print((char*)"Testing Memory Lib...\n");
    
    // Test malloc/free
    char* p1 = (char*)malloc(10);
    if (p1 != 0) {
        print((char*)"  malloc: OK\n");
        free(p1);
        print((char*)"  free: OK\n");
    } else {
        print((char*)"  malloc: FAIL\n");
    }
    
    // Test memset
    char buffer[10];
    memset(buffer, 'A', 9);
    buffer[9] = 0;
    if (buffer[0] == 'A' && buffer[8] == 'A') {
        print((char*)"  memset: OK\n");
    } else {
        print((char*)"  memset: FAIL\n");
    }
    
    // Test memcpy
    char src[] = "CopyMe";
    char dest[10];
    memcpy(dest, src, 7);
    if (strcmp(dest, src) == 0) {
        print((char*)"  memcpy: OK\n");
    } else {
        print((char*)"  memcpy: FAIL\n");
    }
}

void run_libc_tests() {
    print((char*)"--- Running LibC Tests ---\n");
    test_string();
    test_memory();
    print((char*)"--- Tests Finished ---\n");
}
