#ifndef SFS_FILE_H
#define SFS_FILE_H

#include <unit_types.h>
#include <drivers/block_device.h>
#include "superblock.h"
#include "simplefs_internals.h"

ssize_t sfs_file_read(block_device_t* device, inode_t* inode, void* buffer, size_t count, uint32_t offset);
ssize_t sfs_file_write(block_device_t* device, superblock_t* sb, inode_t* inode, void* buffer, size_t count, uint32_t offset);
int sfs_file_truncate(block_device_t* device, superblock_t* sb, inode_t* inode, uint32_t size);
#endif /* SFS_FILE_H */