#ifndef SIMPLEFS_DIR_H
#define SIMPLEFS_DIR_H

#include <unit_types.h>
#include <drivers/block_device.h>
#include "superblock.h"
#include "simplefs_internals.h"

int sfs_create_dir_entry(superblock_t* sb, inode_t* dir_node, const char* name, uint32_t inode_num, uint8_t type);
int sfs_remove_dir_entry(superblock_t* sb, inode_t* dir_node, const char* name);
int sfs_lookup_dir_entry(superblock_t* sb, inode_t* dir_node, const char* name, uint32_t* out_inode_num);
int sfs_list_dir_entries(superblock_t* sb, inode_t* dir_node, void (*callback)(dir_entry_t*, void*), void* arg);
int sfs_is_dir_empty(superblock_t* sb, inode_t* dir_node);
#endif /* SIMPLEFS_DIR_H */