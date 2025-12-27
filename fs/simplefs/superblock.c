#include <drivers/block_device.h>
#include <libc/mem.h>
#include <kernel/mem.h>
#include <errno.h>
#include "simplefs_internals.h"

int sfs_read_superblock(block_device_t* device, superblock_t* sb){
    uint8_t* buffer = (uint8_t*)kmalloc(512);

    if (device->read(device, 1, buffer, 512) < 0){
        kfree(buffer);
        return -EIO;
    }
    memcpy(sb, buffer, sizeof(superblock_t));
    kfree(buffer);
    return 0;
}

int sfs_write_superblock(block_device_t* device, superblock_t* sb){
    uint8_t* buffer = (uint8_t*)kmalloc(512);
    memset(buffer, 0, 512);
    memcpy(buffer, sb, sizeof(superblock_t));

    if (device->write(device, 1, buffer, 512) < 0){
        kfree(buffer);
        return -EIO;
    }
    kfree(buffer);
    return 0;
}

int sfs_validate_superblock(superblock_t* sb){
    if (sb->magic != SIMPLEFS_MAGIC){
        return -EFTYPE;
    }

    if (sb->block_size == 0 || sb->block_count == 0 ||
        sb->inode_count == 0){
        return -EINVAL;
    }

    if (sb->free_blocks > sb->block_count){
        return -EINVAL;
    }

    if (sb->free_inodes > sb->inode_count){
        return -EINVAL;
    }

    return 0;
}