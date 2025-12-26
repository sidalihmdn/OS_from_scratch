#include <unit_types.h>
#include "simplefs_internals.h"


uint32_t sfs_alloc_block(superblock_t* sb){
    // takes the superblock as a parameter to look at the bitmap
    // and find the first free block
    return 0;
}

void sfs_free_block(superblock_t* sb, uint32_t block_num){
    // takes the superblock as a parameter to look at the bitmap
    // and free the block
}

uint32_t sfs_get_block(inode_t* node, uint32_t block_index){
    // get the block from the inode
    return 0;
} 

uint32_t sfs_map_block(inode_t* node, uint32_t block_index, bool create){
    // map the block to the inode
    return 0;
}
    