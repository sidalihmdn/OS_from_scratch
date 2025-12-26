#ifndef SYS_STAT_H
#define SYS_STAT_H

/*
 * File type mask
 */
#define S_IFMT   0170000  /* bitmask for the file type */

/*
 * File types
 */
#define S_IFIFO  0010000  /* FIFO */
#define S_IFCHR  0020000  /* Character device */
#define S_IFDIR  0040000  /* Directory */
#define S_IFBLK  0060000  /* Block device */
#define S_IFREG  0100000  /* Regular file */
#define S_IFLNK  0120000  /* Symbolic link */
#define S_IFSOCK 0140000  /* Socket */

/*
 * Special permission bits
 */
#define S_ISUID  04000    /* Set-user-ID */
#define S_ISGID  02000    /* Set-group-ID */
#define S_ISVTX  01000    /* Sticky bit */

/*
 * Owner permissions
 */
#define S_IRUSR  0400     /* Read */
#define S_IWUSR  0200     /* Write */
#define S_IXUSR  0100     /* Execute */

/*
 * Group permissions
 */
#define S_IRGRP  0040
#define S_IWGRP  0020
#define S_IXGRP  0010

/*
 * Others permissions
 */
#define S_IROTH  0004
#define S_IWOTH  0002
#define S_IXOTH  0001

/*
 * Common permission combinations
 */
#define S_IRWXU  (S_IRUSR | S_IWUSR | S_IXUSR)
#define S_IRWXG  (S_IRGRP | S_IWGRP | S_IXGRP)
#define S_IRWXO  (S_IROTH | S_IWOTH | S_IXOTH)

/*
 * File type test macros
 */
#define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)  (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)

#endif /* SYS_STAT_H */