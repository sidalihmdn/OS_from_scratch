#ifndef FS_H
#define FS_H

#include <unit_types.h>

#define SIMPLEFS_MAGIC 0x12345678
#define SIMPLEFS_MAX_DIRECT 12

typedef struct {
  uint32_t magic;
  uint32_t block_count;
  uint32_t block_size;
  uint32_t inode_count;
  uint32_t free_blocks;
  uint32_t free_inodes;
  uint32_t first_data_block;
  uint32_t inode_bitmap_block;
  uint32_t data_bitmap_block;
  uint32_t inode_table_block;
} superblock_t;

typedef struct {
  uint32_t mode;
  uint32_t uid;
  uint32_t size;
  uint32_t atime;
  uint32_t mtime;
  uint32_t ctime;
  uint32_t n_links;
  uint32_t n_blocks;
  uint32_t blocks[12];
  uint32_t indirect;
  uint8_t reserved[44];
} inode_t;

typedef struct {
  uint32_t inode;
  uint16_t rec_len;
  uint8_t name_len;
  uint8_t file_type;
  char name[256];
} dir_entry_t;

#endif