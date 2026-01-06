#ifndef SFS_OP_TABLE_H
#define SFS_OP_TABLE_H

#include <fs/vfs.h>

extern inode_ops_t sfs_inode_ops;
extern file_ops_t sfs_file_ops;
extern super_ops_t sfs_super_ops;
#endif /* SFS_OP_TABLE_H */