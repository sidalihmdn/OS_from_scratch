#include <unit_types.h>
#include <drivers/block_device.h>
#include <drivers/clock.h>
#include <kernel/mem.h>
#include <libc/mem.h>
#include <libc/string.h>
#include <errno.h>
#include <sys/stat.h>
#include "simplefs_internals.h"
#include "superblock.h"
#include "block.h"
#include "inode.h"
#include "dir.h"


int sfs_create_dir_entry(superblock_t* sb, inode_t* dir_node, const char* name, uint32_t inode_num, uint8_t type){
    /* check if the entry exists */
    uint32_t existing_inode;
    if (sfs_lookup_dir_entry(sb, dir_node, name, &existing_inode) == 0){
        return -EEXIST;
    }

    uint8_t name_len = strlen(name);
    uint32_t rec_len = ALIGN(8 + name_len, 4);
    uint32_t offset = dir_node->size;
    uint32_t i_block = offset / SIMPLEFS_BLOCK_SIZE;
    uint32_t block_offset = offset % SIMPLEFS_BLOCK_SIZE;

    /* check if the dir entry fits at the end of the physical block */
    if (block_offset + rec_len > SIMPLEFS_BLOCK_SIZE){
        /* need to allocate a new block */
        i_block++;
        block_offset = 0;
        offset = i_block * SIMPLEFS_BLOCK_SIZE;
    }

    /* get physical block if does not exist allocate */    
    uint32_t phys_block = sfs_get_block(sb, dir_node, i_block);
    if (phys_block == 0){
        return -ENOSPC;
    }

    /* read block data */
    uint8_t* block_data = (uint8_t*)kmalloc(SIMPLEFS_BLOCK_SIZE);
    if (sb->device->read(sb->device, phys_block, block_data, SIMPLEFS_BLOCK_SIZE) < 0){
        kfree(block_data);
        return -EIO;
    }

    /* create dir entry */
    dir_entry_t* entry = (dir_entry_t*)(block_data + block_offset);
    entry->inode = inode_num;
    entry->rec_len = rec_len;
    entry->name_len = name_len;
    entry->file_type = type;
    memcpy(entry->name, name, name_len);

    /* write block data back */
    if (sb->device->write(sb->device, phys_block, block_data, SIMPLEFS_BLOCK_SIZE) < 0){
        kfree(block_data);
        return -EIO;
    }
    kfree(block_data);

    /* update dir_node size */
    dir_node->size += rec_len;
    dir_node->mtime = clock_get_unix_timestamp();
    if (sfs_write_inode(sb, inode_num, dir_node) < 0){
        return -EIO;
    }
    return 0;
}

int sfs_remove_dir_entry(superblock_t* sb, inode_t* dir_node, const char* name){
    /* check if the node is a directory */
    if ((dir_node->mode & S_IFMT) != S_IFDIR){
        return -ENOTDIR;
    }

    if (!strcmp(name, ".") || !strcmp(name, "..")){
        return -EINVAL;
    }

    uint8_t* buffer = (uint8_t*)kmalloc(SIMPLEFS_BLOCK_SIZE);
    uint32_t name_len = strlen(name);
    uint32_t offset = 0;
    
    while(offset < dir_node->size){
        uint32_t i_block = offset / SIMPLEFS_BLOCK_SIZE;
        uint32_t block_offset = offset % SIMPLEFS_BLOCK_SIZE;
        uint32_t phys_block = sfs_get_block(sb, dir_node, i_block);

        /* the block does not exist on the node*/
        if (phys_block == 0){
            offset += SIMPLEFS_BLOCK_SIZE;
            continue;

        }

        if(sb->device->read(sb->device, phys_block, buffer, SIMPLEFS_BLOCK_SIZE) < 0){
            kfree(buffer);
            return -EIO;
        }
        dir_entry_t* prev_entry = NULL;

        while (block_offset < SIMPLEFS_BLOCK_SIZE && offset < dir_node->size){
            dir_entry_t* dir_entry = (dir_entry_t*)(buffer + block_offset);

            /* safety guard */
            if (dir_entry->rec_len == 0 || dir_entry->rec_len > SIMPLEFS_BLOCK_SIZE - block_offset){
                kfree(buffer);
                return -EIO;
            }

            /* cheeck for match */
            if (dir_entry->inode != 0 && dir_entry->name_len == name_len){
                if (memcmp(dir_entry->name, name, name_len) == 0){
                    if (prev_entry){
                        prev_entry->rec_len += dir_entry->rec_len;
                        dir_entry->inode = 0;
                    } 
                    else{
                        dir_entry->inode = 0;
                    }
                }

                /* write the block back */
                if(sb->device->write(sb->device, phys_block, buffer, SIMPLEFS_BLOCK_SIZE) < 0){
                    kfree(buffer);
                    return -EIO;
                }

                dir_node->mtime = clock_get_unix_timestamp();
                dir_node->ctime = dir_node->mtime;
                return 0;
            }
            prev_entry = dir_entry;
            block_offset += dir_entry->rec_len;
            offset += dir_entry->rec_len;
        }
    }
    /* not found */
    kfree(buffer);
    return -ENOENT;
}

int sfs_lookup_dir_entry(superblock_t* sb, inode_t* dir_node, const char* name, uint32_t* out_inode_num){

    /* check if dir_node is a directory */
    if ((dir_node->mode & S_IFMT) != S_IFDIR){
        return -ENOTDIR;
    }


    uint8_t* block_data = (uint8_t*)kmalloc(SIMPLEFS_BLOCK_SIZE);
    uint8_t name_len = strlen(name);
    uint32_t offset = 0;

    /* iterate over directory entries */
    while(offset < dir_node->size){
        uint32_t i_block = offset / SIMPLEFS_BLOCK_SIZE;
        uint32_t block_offset = offset % SIMPLEFS_BLOCK_SIZE;
        uint32_t physical_block = sfs_get_block(sb, dir_node, i_block);
        /* block not allocated - go to next block */
        if (physical_block == 0){
            offset += SIMPLEFS_BLOCK_SIZE;
            continue;
        }

        /* read block data */
        if (sb->device->read(sb->device, physical_block, block_data, SIMPLEFS_BLOCK_SIZE) < 0){
            kfree(block_data);
            return -EIO;
        }

        /* iterate over directory entries in the block */
        while ( (offset/ SIMPLEFS_BLOCK_SIZE) == i_block && offset < dir_node->size){

            /* get directory entry */
            dir_entry_t* entry = (dir_entry_t*)(block_data + block_offset);
            if (entry->inode != 0 && entry->name_len == name_len){
                if (memcmp(entry->name, name, name_len) == 0){
                    *out_inode_num = entry->inode;
                    kfree(block_data);
                    return 0;
                }
            }
            offset += entry->rec_len;
            block_offset += entry->rec_len;
        }
    }
    /* not found */
    kfree(block_data);
    return -ENOENT;
}

int sfs_list_dir_entries(superblock_t* sb, inode_t* dir_node, void(*callback)(void* entry, void* context), void* context){
    /* check if dir_node is a directory */
    if ((dir_node->mode & S_IFMT) != S_IFDIR){
        return -ENOTDIR;
    }

    uint8_t* block_data = (uint8_t*)kmalloc(SIMPLEFS_BLOCK_SIZE);
    uint32_t offset = 0;

    /* iterate over directory entries */
    while(offset < dir_node->size){
        uint32_t i_block = offset / SIMPLEFS_BLOCK_SIZE;
        uint32_t block_offset = offset % SIMPLEFS_BLOCK_SIZE;
        uint32_t physical_block = sfs_get_block(sb, dir_node, i_block);

        /* block not allocated - go to next block */
        if (physical_block == 0){
            offset += SIMPLEFS_BLOCK_SIZE;
            continue;
        }

        /* read block data */
        if (sb->device->read(sb->device, physical_block, block_data, SIMPLEFS_BLOCK_SIZE) < 0){
            kfree(block_data);
            return -EIO;
        }

        /* iterate over directory entries in the block */
        while ( (offset/ SIMPLEFS_BLOCK_SIZE) == i_block && offset < dir_node->size){

            /* get directory entry */
            dir_entry_t* entry = (dir_entry_t*)(block_data + block_offset);
            if (entry->rec_len == 0 || entry->rec_len > SIMPLEFS_BLOCK_SIZE - block_offset) {
                kfree(block_data);
                return -EIO;
            }

            if (entry->inode != 0){
                callback(entry, context);
            }
            offset += entry->rec_len;
            block_offset += entry->rec_len;
        }
    }
    kfree(block_data);
    return 0;
}
   

int sfs_is_dir_empty(superblock_t* sb, inode_t* dir_node){
    uint8_t* block_data = (uint8_t*)kmalloc(SIMPLEFS_BLOCK_SIZE);
    uint32_t offset = 0;

    /* iterate over directory entries */
    while(offset < dir_node->size){
        uint32_t i_block = offset / SIMPLEFS_BLOCK_SIZE;
        uint32_t block_offset = offset % SIMPLEFS_BLOCK_SIZE;
        uint32_t physical_block = sfs_get_block(sb, dir_node, i_block);

        /* block not allocated - go to next block */
        if (physical_block == 0){
            offset += SIMPLEFS_BLOCK_SIZE;
            continue;
        }

        /* read block data */
        if (sb->device->read(sb->device, physical_block, block_data, SIMPLEFS_BLOCK_SIZE) < 0){
            kfree(block_data);
            return 0;
        }

        /* iterate over directory entries in the block */
        while ( (offset/ SIMPLEFS_BLOCK_SIZE) == i_block && offset < dir_node->size){

            /* get directory entry */
            dir_entry_t* entry = (dir_entry_t*)(block_data + block_offset);
            if (entry->rec_len == 0 || entry->rec_len > SIMPLEFS_BLOCK_SIZE - block_offset) {
                kfree(block_data);
                return 0;
            }

            if (entry->name_len > 0 && entry->inode != 0){
                if (strcmp(entry->name, ".") == 0 || strcmp(entry->name, "..") == 0){
                    /* skip . and .. entries */
                } else {
                    kfree(block_data);
                    return 0; /* not empty */
                }
            }
            offset += entry->rec_len;
            block_offset += entry->rec_len;
        }
    }
    kfree(block_data);
    return 1; /* empty */
}