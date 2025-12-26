#include <unit_types.h>
#include "simplefs_internals.h"


int sfs_create_dir_entry(inode_t* dir_node, const char* name, uint32_t inode_num, uint8_t type){
    return -1;
}

int sfs_remove_dir_entry(inode_t* dir_node, const char* name){
    return -1;
}

int sfs_lookup_dir_entry(inode_t* dir_node, const char* name){
    return -1;
}

int sfs_list_dir_entries(inode_t* dir_node, void(*callback)(dir_entry_t* entry)){
    return -1;
}
    