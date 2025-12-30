#include <unit_types.h>
#include <drivers/block_device.h>
#include <kernel/mem.h>
#include <libc/mem.h>
#include <errno.h>
#include "bitmap.h"
#include "simplefs_internals.h"
#include "superblock.h"


uint32_t sfs_alloc_block(block_device_t* device, superblock_t* sb){
    
    if (sb->free_blocks == 0){
        return 0; // no free blocks
    }

    uint32_t total_blocks = sb->block_count;
    uint32_t bitmap_block = sb->data_bitmap_block;
    uint32_t bitmap_size = (total_blocks + 7) / 8;
    uint8_t* bitmap = (uint8_t*)kmalloc(bitmap_size);


    // read bitmap from disk
    if (device->read(device, bitmap_block, bitmap, bitmap_size) < 0){
        kfree(bitmap);
        return 0;
    }
    
    // find a free block using bitmap_alloc
    int block_num = bitmap_alloc(bitmap, total_blocks);
    if (block_num < 1){
        kfree(bitmap);
        return 0; // no free blocks
    }

    // write bitmap back to disk
    if (device->write(device, bitmap_block, bitmap, bitmap_size) < 0){
        kfree(bitmap);
        return 0;
    }

    // update superblock free_blocks count
    sb->free_blocks--;
    if (sfs_write_superblock(device, sb) < 0){
        kfree(bitmap);
        return 0;
    }

    // return allocated block number
    kfree(bitmap);
    return block_num;
}

void sfs_free_block(block_device_t* device, superblock_t* sb, uint32_t block_num){
    if (block_num >= sb->block_count){
        return; // invalid block number
    }

    uint32_t total_blocks = sb->block_count;
    uint32_t bitmap_block = sb->data_bitmap_block;
    uint32_t bitmap_size = (total_blocks + 7) / 8;
    uint8_t* bitmap = (uint8_t*)kmalloc(bitmap_size);

    // read bitmap from disk
    if (device->read(device, bitmap_block, bitmap, bitmap_size) < 0){
        kfree(bitmap);
        return;
    }

    // free the block in the bitmap
    bitmap_free(bitmap, block_num);

    // write bitmap back to disk
    if (device->write(device, bitmap_block, bitmap, bitmap_size) < 0){
        kfree(bitmap);
        return;
    }

    // update superblock free_blocks count
    sb->free_blocks++;
    sfs_write_superblock(device, sb);
    kfree(bitmap);
}

/// @brief  Get the physical block number for a given logical block index in an inode.  
/// @param device 
/// @param node 
/// @param block_index 
/// @return physical block number, or 0 if not allocated 
uint32_t sfs_get_block(block_device_t* device, inode_t* node, uint32_t block_index){

    /* direct blocks*/
    if (block_index < SIMPLEFS_MAX_DIRECT){
        return node->blocks[block_index];
    }

    /* indirect blocks */
    block_index -= SIMPLEFS_MAX_DIRECT;
    uint32_t ptr_per_block = SIMPLEFS_BLOCK_SIZE / sizeof(uint32_t);
    if (block_index < ptr_per_block){
        uint8_t* indirect_block= (uint8_t*)kmalloc(SIMPLEFS_BLOCK_SIZE);
        if (device->read(device, node->indirect, indirect_block, SIMPLEFS_BLOCK_SIZE) < 0){
            kfree(indirect_block);
            return 0;
        }
        uint32_t block = ((uint32_t*)indirect_block)[block_index];
        kfree(indirect_block);
        return block;
    }
    /* beyond file size
     * TODO : implement double indirect blocks or triple indirect blocks
     */
    return 0;
} 

uint32_t sfs_map_block(block_device_t* device, superblock_t* sb, inode_t* inode, uint32_t block_index, int create){
    uint32_t physical_block = sfs_get_block(device, inode, block_index);
    if (physical_block != 0 || !create){
        return physical_block;
    }

    physical_block = sfs_alloc_block(device, sb);
    if (physical_block == 0){
        return 0; // no space
    }

    // zero out the newly allocated block
    uint8_t* zero_block = (uint8_t*)kmalloc(SIMPLEFS_BLOCK_SIZE);
    memset(zero_block, 0, SIMPLEFS_BLOCK_SIZE);
    if (device->write(device, physical_block,  zero_block, SIMPLEFS_BLOCK_SIZE) < 0){
        kfree(zero_block);
        sfs_free_block(device, sb, physical_block);
        return 0; // write error
    }
    kfree(zero_block);

    if (block_index < SIMPLEFS_MAX_DIRECT){
        inode->blocks[block_index] = physical_block;
        inode->n_blocks++;
        return physical_block;
    }

    block_index -= SIMPLEFS_MAX_DIRECT;
    uint32_t ptr_per_block = SIMPLEFS_BLOCK_SIZE / sizeof(uint32_t);
    if (block_index < ptr_per_block){
        if (inode->indirect == 0){
            inode->indirect = sfs_alloc_block(device, sb);
            if (inode->indirect == 0){
                sfs_free_block(device, sb, physical_block);
                return 0; // no space
            }
            uint8_t* zero_block = (uint8_t*)kmalloc(SIMPLEFS_BLOCK_SIZE);
            memset(zero_block, 0, SIMPLEFS_BLOCK_SIZE);
            if (device->write(device, inode->indirect, zero_block, SIMPLEFS_BLOCK_SIZE) < 0){
                kfree(zero_block);
                sfs_free_block(device, sb, physical_block);
                sfs_free_block(device, sb, inode->indirect);
                inode->indirect = 0;
                return 0; // write error
            }
            inode->n_blocks++;
            kfree(zero_block);
        }
        uint8_t* indirect_block= (uint8_t*)kmalloc(SIMPLEFS_BLOCK_SIZE);
        if (device->read(device, inode->indirect, indirect_block, SIMPLEFS_BLOCK_SIZE) < 0){
            kfree(indirect_block);
            sfs_free_block(device, sb, physical_block);
            return 0; // read error
        }
        ((uint32_t*)indirect_block)[block_index] = physical_block;
        if (device->write(device, inode->indirect, indirect_block, SIMPLEFS_BLOCK_SIZE) < 0){
            kfree(indirect_block);
            sfs_free_block(device, sb, physical_block);
            return 0; // write error
        }
        inode->n_blocks++;
        kfree(indirect_block);
        return physical_block;
    }
    /* beyond file size
     * TODO : implement double indirect blocks or triple indirect blocks
     */
     sfs_free_block(device, sb, physical_block);
    return 0;
}
    