#ifndef FS_H
#define FS_H

#include <unit_types.h>

#define SIMPLEFS_MAGIC 0x12345678
#define SIMPLEFS_MAX_DIRECT 12

typedef struct {
    uint32_t magic;
    uint32_t block_count;
    uint32_t block_size;
    uint32_t free_blocks;
    uint32_t inode_start;
    uint32_t root_inode;
} superblock_t;


typedef struct {
    uint32_t size;
    uint32_t blocks[SIMPLEFS_MAX_DIRECT];
    uint32_t indirect_block;
    uint8_t type;
} inode_t;

typedef struct {
    char name[256];
    uint32_t inode;
} dir_entry_t;

#endif