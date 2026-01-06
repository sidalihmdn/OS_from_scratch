#include <unit_types.h>
#include <drivers/block_device.h>
#include <kernel/mem.h>
#include <libc/mem.h>
#include <drivers/clock.h>
#include <fs/vfs.h>
#include <sys/stat.h>
#include <errno.h>
#include "block.h"
#include "inode.h"
#include "simplefs_internals.h"
#include "sfs_op_table.h"
#include "superblock.h"
#include "dir.h"
#include "file.h"

/**
 * =====================================================================
 * super operations
 * =====================================================================
*/

int sfs_read_inode(vfs_superblock_t* sb, uint32_t inode_num, vfs_inode_t* inode){
    return sfs_read_inode((superblock_t*)sb->s_fs, inode_num, (inode_t*)inode->i_fs);
}

int sfs_write_inode(vfs_superblock_t* sb, uint32_t inode_num, vfs_inode_t* inode){
    return sfs_write_inode((superblock_t*)sb->s_fs, inode_num, (inode_t*)inode->i_fs);
}

void sfs_put_superblock(vfs_superblock_t* sb){
    superblock_t* sfs_sb = (superblock_t*)sb->s_fs;
    kfree(sfs_sb);
    kfree(sb);
}

super_ops_t sfs_super_ops = {
    .read_inode = sfs_read_inode,
    .write_inode = sfs_write_inode,
    .put_superblock = sfs_put_superblock,
    .sync_fs = NULL,
    .statfs = NULL,
    .mount_fs = NULL
};


/**
 * =====================================================================
 * inode operations
 * =====================================================================
*/

int sfs_lookup(vfs_inode_t* dir, vfs_dir_entry_t* entry){
    uint32_t out_inode_num;
    int res = sfs_lookup_dir_entry((superblock_t*)dir->i_sb->s_fs,
                                      (inode_t*)dir->i_fs,
                                      entry->d_name,
                                      &out_inode_num);
    if (res < 0){
        return res;
    }

    vfs_inode_t* vfs_inode = (vfs_inode_t*)kmalloc(sizeof(vfs_inode_t));
    if (!vfs_inode){
        return -ENOMEM;
    }

    inode_t* sfs_inode = (inode_t*)kmalloc(sizeof(inode_t));
    if (!sfs_inode){
        kfree(vfs_inode);
        return -ENOMEM;
    }
    res = sfs_read_inode((superblock_t*)dir->i_sb->s_fs, out_inode_num, sfs_inode);
    if (res < 0){
        kfree(sfs_inode);
        kfree(vfs_inode);
        return res; 
    }

    vfs_inode->i_number = out_inode_num;
    vfs_inode->i_mode = sfs_inode->mode;
    vfs_inode->i_size = sfs_inode->size;
    vfs_inode->i_uid = sfs_inode->uid;
    vfs_inode->i_gid = 0; /* SimpleFS does not store gid */
    vfs_inode->i_atime = sfs_inode->atime;
    vfs_inode->i_mtime = sfs_inode->mtime;
    vfs_inode->i_ctime = sfs_inode->ctime;
    vfs_inode->i_links = sfs_inode->n_links;
    vfs_inode->i_blocks = sfs_inode->n_blocks;
    vfs_inode->i_sb = dir->i_sb;
    vfs_inode->i_op = &sfs_inode_ops;
    vfs_inode->i_f_op = &sfs_file_ops;
    vfs_inode->i_fs = sfs_inode;
    vfs_inode->i_stats = 0; /* clean inode */

    entry->d_inode = vfs_inode;
    return 0;
}

int sfs_create(vfs_inode_t* dir, vfs_dir_entry_t* entry, uint32_t mode){
    superblock_t* sb = (superblock_t*)dir->i_sb->s_fs;
    inode_t* dir_node = (inode_t*)dir->i_fs;
    uint32_t new_inode_num = sfs_alloc_inode(sb);
    if (new_inode_num == 0){
        return -ENOSPC;
    }
    inode_t* sfs_inode  = (inode_t*)kmalloc(sizeof(inode_t));
    if (!sfs_inode){
        sfs_free_inode(sb, new_inode_num);
        return -ENOMEM;
    }
    sfs_inode->mode = mode | S_IFREG;
    sfs_inode->uid = dir->i_uid;
    sfs_inode->size = 0; 
    sfs_inode->atime = sfs_inode->mtime = sfs_inode->ctime = clock_get_unix_timestamp();
    sfs_inode->n_links = 1;
    sfs_inode->n_blocks = 0;
    memset(sfs_inode->blocks, 0, sizeof(sfs_inode->blocks));
    sfs_inode->indirect = 0;

    int res = sfs_write_inode(sb, new_inode_num, sfs_inode);
    if (res < 0){
        kfree(sfs_inode);
        sfs_free_inode(sb, new_inode_num);
        return res;
    }

    res = sfs_create_dir_entry(sb, dir_node, entry->d_name, new_inode_num, SFS_FT_REG_FILE);
    if (res < 0){
        kfree(sfs_inode);
        sfs_free_inode(sb, new_inode_num);
        return res;
    }

    vfs_inode_t* vfs_inode = (vfs_inode_t*)kmalloc(sizeof(vfs_inode_t));
    if (!vfs_inode){
        kfree(sfs_inode);
        sfs_free_inode(sb, new_inode_num);
        return -ENOMEM; 
    }

    vfs_inode->i_number = new_inode_num;
    vfs_inode->i_mode = sfs_inode->mode;
    vfs_inode->i_size = sfs_inode->size;
    vfs_inode->i_uid = sfs_inode->uid;
    vfs_inode->i_gid = 0; /* SimpleFS does not store gid */
    vfs_inode->i_atime = sfs_inode->atime;
    vfs_inode->i_mtime = sfs_inode->mtime;
    vfs_inode->i_ctime = sfs_inode->ctime;
    vfs_inode->i_links = sfs_inode->n_links;
    vfs_inode->i_blocks = sfs_inode->n_blocks;
    vfs_inode->i_sb = dir->i_sb;
    vfs_inode->i_op = &sfs_inode_ops;
    vfs_inode->i_f_op = &sfs_file_ops;
    vfs_inode->i_fs = sfs_inode;
    vfs_inode->i_stats = 0; /* clean inode */

    entry->d_inode = vfs_inode;

    dir->i_mtime = clock_get_unix_timestamp();
    dir->i_stats |= 0x1; /* mark dir inode as dirty */
    return 0;
}

int sfs_mkdir(vfs_inode_t* dir, vfs_dir_entry_t* entry, uint16_t mode){
    superblock_t* sb = (superblock_t*)dir->i_sb->s_fs;
    inode_t* dir_node = (inode_t*)dir->i_fs;

    /* allocate new inode */
    uint32_t new_inode_num = sfs_alloc_inode(sb);
    if (new_inode_num == 0){
        return -ENOSPC;
    }

    /* initialize new inode */
    inode_t* sfs_inode  = (inode_t*)kmalloc(sizeof(inode_t));
    if (!sfs_inode){
        sfs_free_inode(sb, new_inode_num);
        return -ENOMEM;
    }
    sfs_inode->mode = mode | S_IFDIR;
    sfs_inode->uid = dir->i_uid;
    sfs_inode->size = 0;
    sfs_inode->atime = sfs_inode->mtime = sfs_inode->ctime = clock_get_unix_timestamp();
    sfs_inode->n_links = 2; /* . and .. */
    sfs_inode->n_blocks = 0;
    memset(sfs_inode->blocks, 0, sizeof(sfs_inode->blocks));
    sfs_inode->indirect = 0;

    int res = sfs_write_inode(sb, new_inode_num, sfs_inode);
    if (res < 0){
        kfree(sfs_inode);
        sfs_free_inode(sb, new_inode_num);
        return res;
    }

    /* create dir entry in parent directory */
    res = sfs_create_dir_entry(sb, dir_node, entry->d_name, new_inode_num, SFS_FT_DIR);
    if(res < 0) {
        kfree(sfs_inode);
        sfs_free_inode(sb, new_inode_num);
        return res;
    }

    /* creatre "." entry */
    res = sfs_create_dir_entry(sb, sfs_inode, ".", new_inode_num, SFS_FT_DIR);
    if(res < 0) {
        sfs_remove_dir_entry(sb, dir_node, entry->d_name);
        kfree(sfs_inode);
        sfs_free_inode(sb, new_inode_num);
        return res;
    }

    /* creatre ".." entry */
    res = sfs_create_dir_entry(sb, sfs_inode, "..", new_inode_num, SFS_FT_DIR);
    if(res < 0) {
        sfs_remove_dir_entry(sb, sfs_inode, ".");
        sfs_remove_dir_entry(sb, dir_node, entry->d_name);
        kfree(sfs_inode);
        sfs_free_inode(sb, new_inode_num);
        return res;
    }

    dir->i_links++;
    dir->i_stats |= 0x1;

    return 0;
}

int sfs_rmdir(vfs_inode_t* dir, vfs_dir_entry_t* entry){
    superblock_t* sb = (superblock_t*)dir->i_sb->s_fs;
    inode_t* dir_node = (inode_t*)dir->i_fs;

    if ((entry->d_inode->i_mode & S_IFMT) != S_IFDIR){
        return -ENOTDIR; /* not a directory */
    }
    if(sfs_is_dir_empty((superblock_t*)dir->i_sb->s_fs, (inode_t*)entry->d_inode->i_fs) == 0){
        return -ENOTEMPTY; /* directory not empty */
    }
    return 0;

}

int sfs_unlink(vfs_inode_t* dir, vfs_dir_entry_t* entry){
    return -ENOSYS;
}

int sfs_rename(vfs_inode_t* old_dir, vfs_dir_entry_t* old_entry,
               vfs_inode_t* new_dir, vfs_dir_entry_t* new_entry){
    return -ENOSYS;
}

int sfs_link(vfs_inode_t* dir, vfs_dir_entry_t* entry, vfs_inode_t* target){
    return -ENOSYS;
}

int sfs_symlink(vfs_inode_t* dir, vfs_dir_entry_t* entry, const char* target){
    return -ENOSYS;
}

int sfs_truncate(vfs_inode_t* inode, uint32_t size){
    return -ENOSYS;
}

int sfs_permission(vfs_inode_t* inode, uint16_t mode){
    return -ENOSYS;
}

inode_ops_t sfs_inode_ops = {
    .lookup = sfs_lookup
};

/**
 * =====================================================================
 * file operations
 * =====================================================================
*/

ssize_t sfs_file_read(vfs_file_t* file, void* buffer, size_t count, uint32_t* offset){
    return -ENOSYS;
}

ssize_t sfs_file_write(vfs_file_t* file, const void* buffer, size_t count, uint32_t* offset){
    return -ENOSYS;
}

int sfs_file_open(vfs_inode_t* inode, vfs_file_t* file){
    return -ENOSYS;
}

int sfs_file_release(vfs_inode_t* inode, vfs_file_t* file){
    return -ENOSYS;
}

int sfs_file_ioctl(vfs_file_t* file, uint32_t cmd, void* arg){
    return -ENOSYS;
}

int sfs_file_seek(vfs_file_t* file, int32_t offset, uint8_t whence){
    return -ENOSYS;
}

file_ops_t sfs_file_ops = {
    .read = sfs_file_read,
    .write = sfs_file_write,
    .open = NULL,
    .release = NULL,
    .ioctl = NULL,
    .seek = NULL
};

