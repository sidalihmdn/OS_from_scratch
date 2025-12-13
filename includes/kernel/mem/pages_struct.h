#ifndef PAGES_STRUCT_H
#define PAGES_STRUCT_H
typedef struct {
    uint32_t present : 1;
    uint32_t rw : 1;
    uint32_t user : 1;
    uint32_t write_through : 1;
    uint32_t cache_disabled : 1;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t pat : 1;
    uint32_t global : 1;
    uint32_t available : 3;
    uint32_t frame : 20;
} __attribute__((packed)) page_table_entry;

typedef struct {
    uint32_t present : 1;
    uint32_t rw : 1;
    uint32_t user : 1;
    uint32_t write_through : 1;
    uint32_t cache_disabled : 1;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t page_size : 1;
    uint32_t global : 1;
    uint32_t available : 3;
    uint32_t frame : 20;
} __attribute__((packed)) page_directory_entry;

#endif