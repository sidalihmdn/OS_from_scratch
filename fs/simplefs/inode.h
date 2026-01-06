#ifndef SIMPLEFS_INODE_H
#define SIMPLEFS_INODE_H
#include <unit_types.h>
#include "superblock.h"
#include "simplefs_internals.h"

uint32_t sfs_alloc_inode(superblock_t *sb);
void sfs_free_inode(superblock_t *sb, uint32_t inode_num);
int sfs_read_inode(superblock_t *sb, uint32_t inode_num, inode_t *inode);
int sfs_write_inode(superblock_t *sb, uint32_t inode_num, inode_t *inode);
int sfs_set_inode_bitmap(superblock_t *sb, uint32_t inode_num);
int sfs_unmap_block(superblock_t* sb, inode_t* inode, uint32_t block_number);
#endif /* SIMPLEFS_INODE_H */
