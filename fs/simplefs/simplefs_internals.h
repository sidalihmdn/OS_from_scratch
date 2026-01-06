#ifndef FS_H
#define FS_H

#include <unit_types.h>
#include <drivers/block_device.h>

#define SIMPLEFS_MAGIC 0x12345678
#define SIMPLEFS_MAX_DIRECT 12
#define SIMPLEFS_BLOCK_SIZE 4096
#define SIMPLEFS_ROOT_INODE 1

#define SFS_FT_UNKNOWN   0
#define SFS_FT_REG_FILE  1  // Regular file
#define SFS_FT_DIR       2  // Directory
#define SFS_FT_CHRDEV    3  // Character device
#define SFS_FT_BLKDEV    4  // Block device
#define SFS_FT_FIFO      5  // Named pipe
#define SFS_FT_SOCK      6  // Socket
#define SFS_FT_SYMLINK   7  // Symbolic link

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

  block_device_t* device;
} superblock_t;

typedef struct {
  uint32_t mode;
  uint32_t uid;
  uint32_t size;
  uint64_t atime;
  uint64_t mtime;
  uint64_t ctime;
  uint32_t n_links;
  uint32_t n_blocks;
  uint32_t blocks[12];
  uint32_t indirect;
  uint8_t reserved[32];
} inode_t;

typedef struct {
  uint32_t inode;
  uint16_t rec_len;
  uint8_t name_len;
  uint8_t file_type;
  char name[];
} dir_entry_t;


#endif