#include "simplefs_internals.h"
#include <drivers/block_device.h>

uint32_t sfs_alloc_inode(superblock_t *sb) { return 0; }

void sfs_free_inode(superblock_t *sb, uint32_t inode_num) {}

int sfs_read_inode(superblock_t *sb, uint32_t inode_num, inode_t *inode) {
  return 0;
}

int sfs_write_inode(superblock_t *sb, uint32_t inode_num, inode_t *inode) {
  return 0;
}
