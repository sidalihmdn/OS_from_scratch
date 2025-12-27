#include <unit_types.h>
#include <drivers/block_device.h>
#include <libc/mem.h>
#include <kernel/mem.h>
#include <errno.h>
#include <sys/stat.h>
#include <drivers/clock.h>
#include "simplefs_internals.h"
#include "superblock.h"
#include "inode.h"

int mkfs(block_device_t *device, uint32_t block_size, uint32_t total_blocks) {
  superblock_t sb;
  uint32_t n_inodes = total_blocks / 4;
  uint32_t inode_bitmap_blocks = (n_inodes + (block_size*8 - 1))/(block_size*8);
  uint32_t data_bitmap_blocks = (total_blocks + (block_size*8 -1))/(block_size*8);
  uint32_t inodes_per_block = block_size / sizeof(inode_t);
  uint32_t inode_table_blocks = (n_inodes + inodes_per_block -1)/inodes_per_block;

  sb.magic = SIMPLEFS_MAGIC;
  sb.block_size = block_size;
  sb.block_count = total_blocks;
  sb.inode_count = n_inodes;
  sb.inode_bitmap_block = 2;
  sb.data_bitmap_block = sb.inode_bitmap_block + inode_bitmap_blocks;
  sb.inode_table_block = sb.data_bitmap_block + data_bitmap_blocks;
  sb.first_data_block = sb.inode_table_block + inode_table_blocks;
  sb.free_blocks = total_blocks - sb.first_data_block;
  sb.free_inodes = n_inodes - 1;

  if (sfs_write_superblock(device, &sb) < 0) {
    return -EIO;
  }

  /* initialize inode & data bitmaps */
  uint8_t* zero_block = (uint8_t *)kmalloc(block_size);
  memset(zero_block, 0, block_size);

  for (uint32_t i = 0; i <inode_bitmap_blocks; i++){
    if (device->write(device, sb.inode_bitmap_block + i, zero_block, block_size) < 0){
      kfree(zero_block);
      return -EIO;
    }
  }

  for (uint32_t i = 0; i <data_bitmap_blocks; i++){
    if (device->write(device, sb.data_bitmap_block + i, zero_block, block_size) < 0){
      kfree(zero_block);
      return -EIO;
    }
  }

  for (uint32_t i = 0; i < inode_table_blocks; i++) {
    if (device->write(device, sb.inode_table_block + i, zero_block, block_size) < 0) {
      kfree(zero_block);
      return -EIO;
    }
  }


  inode_t root_inode;
  memset(&root_inode, 0, sizeof(inode_t));
  root_inode.mode = S_IFDIR | 0x755; /* drwxr-xr-x */
  root_inode.n_links = 2; /* . and .. */
  root_inode.size = 0;
  root_inode.atime = root_inode.mtime = root_inode.ctime = (uint32_t)clock_get_unix_timestamp();

  sfs_write_inode(device, &sb, 1, &root_inode);
  sfs_set_inode_bitmap(device, &sb, 1); /* mark root inode as used */
  kfree(zero_block);
  return 0;
}