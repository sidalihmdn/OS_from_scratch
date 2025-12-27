#ifndef SIMPLEFS_BLOCK_H
#define SIMPLEFS_BLOCK_H

#include <unit_types.h>
#include "superblock.h"
#include "simplefs_internals.h"

uint32_t sfs_alloc_block(superblock_t* sb);
void sfs_free_block(superblock_t* sb, uint32_t block_num);
uint32_t sfs_get_block(inode_t* node, uint32_t block_index); 
uint32_t sfs_map_block(inode_t* node, uint32_t block_index, int create);

#endif /* SIMPLEFS_BLOCK_H */