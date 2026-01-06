#ifndef VFS_H
#define VFS_H
#include <unit_types.h>
#include <drivers/block_device.h>


struct super_ops_t;
struct inode_ops_t;
struct file_ops_t;
/* VFS structures */

typedef struct superblock {
    block_device_t*     s_device;
    uint32_t            s_magic;
    uint32_t            s_block_size;
    uint32_t            s_block_count;
    super_ops_t*        s_op;
    void*               s_fs; /* Filesystem specific superblock data */
} vfs_superblock_t;

typedef struct inode {
    uint32_t            i_number;
    uint32_t            i_mode;
    uint32_t            i_size;
    uint32_t            i_uid;
    uint32_t            i_gid;
    uint64_t            i_atime;
    uint64_t            i_mtime;
    uint64_t            i_ctime;
    uint32_t            i_links;
    uint32_t            i_blocks;
    vfs_superblock_t*   i_sb;
    inode_ops_t*        i_op;
    file_ops_t*         i_f_op;
    void*               i_fs; /* Filesystem specific inode data */
    uint32_t            i_stats; /* inode stats : dirty, etc. */
} vfs_inode_t;


typedef struct file {
    vfs_inode_t*    f_inode;
    uint32_t        f_flags;    /* open flags */
    uint32_t        f_mode;     /* access mode */ 
    uint32_t        f_count;  /* reference count */
    uint32_t        f_pos;    /* current file position */
    file_ops_t*     f_op;
    void*           f_fs; /* Filesystem specific file data */
} vfs_file_t;

typedef struct vfs_dir_entry_t {
    vfs_inode_t*        d_inode;
    vfs_dir_entry_t*    d_parent;
    uint32_t            d_name_len;
    char*               d_name;
} vfs_dir_entry_t;


/** 
 * Operations structures 
 * define function pointers for filesystem operations
 */

typedef struct super_ops_t {
    int (*read_inode)(vfs_superblock_t* sb, uint32_t inode_num, vfs_inode_t* inode);
    int (*write_inode)(vfs_superblock_t* sb, uint32_t inode_num, vfs_inode_t* inode);
    void (*put_superblock)(vfs_superblock_t* sb);
    int (*sync_fs)(vfs_superblock_t* sb);
    int (*statfs)(vfs_superblock_t* sb, void* statfs_buf);
    int (*mount_fs)(vfs_superblock_t* sb, uint32_t* flags, const char* data);
} super_ops_t;

typedef struct inode_ops_t {
    int (*lookup)(vfs_inode_t* dir, vfs_dir_entry_t* entry);
    int (*create)(vfs_inode_t* dir, vfs_dir_entry_t* entry, uint32_t mode);
    int (*mkdir)(vfs_inode_t* dir, vfs_dir_entry_t* entry, uint32_t mode);
    int (*rmdir)(vfs_inode_t* dir, vfs_dir_entry_t* entry);
    int (*unlink)(vfs_inode_t* dir, vfs_dir_entry_t* entry);
    int (*rename)(vfs_inode_t* old_dir, vfs_dir_entry_t* old_entry,
                  vfs_inode_t* new_dir, vfs_dir_entry_t* new_entry);
    int (*link)(vfs_inode_t* dir, vfs_dir_entry_t* entry, vfs_inode_t* target);
    int (*symlink)(vfs_inode_t* dir, vfs_dir_entry_t* entry, const char* target);
    int (*truncate)(vfs_inode_t* inode, uint32_t size);
    int (*permission)(vfs_inode_t* inode, uint32_t mode);
} inode_ops_t;


typedef struct file_ops_t {
    ssize_t (*read)(vfs_file_t* file, void* buffer, size_t count, uint32_t* offset);
    ssize_t (*write)(vfs_file_t* file, const void* buffer, size_t count, uint32_t* offset);
    int (*open)(vfs_inode_t* inode, vfs_file_t* file);
    int (*release)(vfs_inode_t* inode, vfs_file_t* file);
    int (*ioctl)(vfs_file_t* file, uint32_t cmd, void* arg);
    int (*seek)(vfs_file_t* file, int32_t offset, uint8_t whence);
} file_ops_t;


#endif /* VFS_H */