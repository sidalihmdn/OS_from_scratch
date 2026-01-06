#include <unit_types.h>
#include <drivers/block_device.h>
#include <kernel/mem.h>
#include <libc/mem.h>
#include <drivers/clock.h>
#include "superblock.h"
#include "simplefs_internals.h"
#include "inode.h"
#include "block.h"
#include "errno.h"


ssize_t sfs_file_read(superblock_t* sb, inode_t* inode, void* buffer, size_t count, uint32_t offset){
    /* this function reads data from a file represented by the inode into the buffer */

    // check if offset is beyond file size
    if (offset >= inode->size){
        return 0; // EOF
    }

    // adjust count if it goes beyond file size
    if (offset + count > inode->size){
        count = inode->size - offset; // adjust count to read only up to EOF
    }

    // Find on which block to start reading
    uint32_t start_block_i = offset / SIMPLEFS_BLOCK_SIZE;
    uint32_t block_offset = offset % SIMPLEFS_BLOCK_SIZE;
    uint32_t bytes_read = 0;
    uint8_t* data_block = (uint8_t*)kmalloc(SIMPLEFS_BLOCK_SIZE);

    if (!data_block){
        return -ENOMEM;
    }

    // read loop 
    while (bytes_read < count){
        uint32_t physical_block = sfs_get_block(sb, inode, start_block_i);
        uint32_t remaining = count - bytes_read;
        uint32_t to_read = MIN(remaining, SIMPLEFS_BLOCK_SIZE - block_offset);
        if (physical_block == 0){
            memset((uint8_t*)buffer + bytes_read, 0, to_read);
        }
        else{
            if (sb->device->read(sb->device, physical_block, data_block, SIMPLEFS_BLOCK_SIZE) < 0){
                kfree(data_block);
                return -EIO;
            }
            memcpy((uint8_t*)buffer + bytes_read, data_block+block_offset, to_read);
        }        
        bytes_read += to_read;
        start_block_i++;
        block_offset = 0;
    }
    kfree(data_block);
    inode->atime = clock_get_unix_timestamp();

    return (ssize_t)bytes_read;
}

ssize_t sfs_file_write(superblock_t* sb, inode_t* inode, void* buffer, size_t count, uint32_t offset){
    /* this function writes data from the buffer into a file represented by the inode */

    if (count == 0){
        return 0;
    }
    if (offset > inode->size){
        return -EINVAL;
    }
   
    uint32_t start_block_i = offset / SIMPLEFS_BLOCK_SIZE;
    uint32_t block_offset = offset % SIMPLEFS_BLOCK_SIZE;
    uint32_t bytes_written = 0;
    uint8_t* data_block = (uint8_t*)kmalloc(SIMPLEFS_BLOCK_SIZE);
    if (!data_block){
        return -ENOMEM;
    }
    while (bytes_written < count){
        uint32_t phys_block = sfs_map_block(sb, inode, start_block_i, 1);
        if (phys_block == 0){
            kfree(data_block);
            return -ENOSPC;
        }
        uint32_t remaining = count - bytes_written;
        uint32_t to_write = MIN(remaining, SIMPLEFS_BLOCK_SIZE - block_offset);
        if (to_write < SIMPLEFS_BLOCK_SIZE || block_offset != 0){
            if (sb->device->read(sb->device, phys_block, data_block, SIMPLEFS_BLOCK_SIZE) < 0){
                kfree(data_block);
                return -EIO;
            }
        }
        memcpy(data_block + block_offset, (uint8_t*)buffer + bytes_written, to_write);
        if (sb->device->write(sb->device, phys_block, data_block, SIMPLEFS_BLOCK_SIZE) < 0){
            kfree(data_block);
            return -EIO;
        }
        bytes_written += to_write;
        start_block_i++;
        block_offset = 0;
    }
    kfree(data_block);
     if (offset + count > inode->size){
        inode->size = offset + count;
    }
    inode->mtime = clock_get_unix_timestamp();
    inode->ctime = inode->mtime;
    return (ssize_t)bytes_written;
}

int sfs_file_truncate(superblock_t* sb, inode_t* inode, uint32_t size){
    /* this function truncates or extends a file to the specified size */

    if (size == inode->size){
        /* the file has the same size, no change */
        return 0;
    }
    if (size < inode->size){
        /* truncate the file 
        * 1. calculate the number of blocks needed for the new size
        * 2. free the blocks beyond the new size
        * 3. update the inode size
        */
        uint32_t current_blocks_nb = (inode->size + SIMPLEFS_BLOCK_SIZE - 1) / SIMPLEFS_BLOCK_SIZE;
        uint32_t new_blocks_nb = (size + SIMPLEFS_BLOCK_SIZE - 1) / SIMPLEFS_BLOCK_SIZE;

        for(uint32_t i = new_blocks_nb; i < current_blocks_nb; i++){
            uint32_t phys_block = sfs_get_block(sb, inode, i);
            if (phys_block == 0){
                // skip unallocated block
                continue;
            }
            sfs_free_block(sb, phys_block);
            sfs_unmap_block(sb, inode, i);
            inode->n_blocks--;
        }
        inode->size = size;
        inode->mtime = clock_get_unix_timestamp();
        inode->ctime = inode->mtime;
        return 0;
    }
    else{
        /* extend the file
        * just update the inode size, blocks will be allocated on write
        */
        inode->size = size;
        inode->mtime = clock_get_unix_timestamp();
        inode->ctime = inode->mtime;
        return 0;
    }
}
    
