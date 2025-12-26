#include <unit_types.h>
#include "simplefs_internals.h"

ssize_t sfs_file_read(inode_t* inode, void* buffer, size_t count, uint32_t offset){
    return -1;
}

ssize_t sfs_file_write(inode_t* inode, void* buffer, size_t count, uint32_t offset){
    return -1;
}

int sfs_file_truncate(inode_t* inode, uint32_t size){
    return -1;
}
    
