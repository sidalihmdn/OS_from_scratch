#include <drivers/block_device.h>
#include <kernel/mem.h>
#include <libc/mem.h>
#include <errno.h>
#include "bitmap.h"
#include "simplefs_internals.h"
#include "superblock.h"

uint32_t sfs_alloc_inode(block_device_t *device, superblock_t *sb) {
  if (sb->free_inodes == 0) {
    return 0; // No free inodes
  }

  uint32_t bitmap_block = sb->inode_bitmap_block;
  uint32_t bitmap_size = (sb->inode_count + 7) / 8;
  uint8_t* bitmap = (uint8_t*)kmalloc(bitmap_size);

  if (device->read(device, bitmap_block, bitmap, bitmap_size) < 0) {
    kfree(bitmap);
    return 0;
  }
  int inode_num = bitmap_alloc(bitmap, sb->inode_count);
  if (inode_num < 1) {
    kfree(bitmap);
    return 0;
  }

  if (device->write(device, bitmap_block, bitmap, bitmap_size) < 0) {
    kfree(bitmap);
    return 0;
  }
  sb->free_inodes--;
  if (sfs_write_superblock(device, sb) < 0) {
    kfree(bitmap);
    return 0; 
  }
  kfree(bitmap);
  return (uint32_t)inode_num;
}

void sfs_free_inode(block_device_t *device, superblock_t *sb, uint32_t inode_num) {
  uint32_t bitmap_block = sb->inode_bitmap_block;
  uint32_t bitmap_size = (sb->inode_count + 7) / 8;
  uint8_t* bitmap = (uint8_t*)kmalloc(bitmap_size);

  if (device->read(device, bitmap_block, bitmap, bitmap_size) < 0) {
    kfree(bitmap);
    return;
  }
  bitmap_free(bitmap, inode_num);

  if (device->write(device, bitmap_block, bitmap, bitmap_size) < 0) {
    kfree(bitmap);
    return;
  }
  sb->free_inodes++;
  sfs_write_superblock(device, sb);
  kfree(bitmap);
}

int sfs_read_inode(block_device_t *device, superblock_t *sb, uint32_t inode_num, inode_t *inode) {
  if (inode_num == 0 || inode_num > sb->inode_count){
    return -EINVAL;
  }
  uint32_t inodes_per_block = sb->block_size / sizeof(inode_t);
  uint32_t inode_offset = inode_num % inodes_per_block;
  uint32_t block_num = sb->inode_table_block + (inode_num / inodes_per_block);

  uint8_t* buffer = (uint8_t*)kmalloc(sb->block_size);
  if (device->read(device, block_num, buffer, sb->block_size) < 0){
    kfree(buffer);
    return -EIO;
  }

  memcpy(inode, buffer + inode_offset, sizeof(inode_t));
  kfree(buffer);
  return 0;
}

int sfs_write_inode(block_device_t *device, superblock_t *sb, uint32_t inode_num, inode_t *inode) {
  /* this function writes */
  
  if (inode_num == 0 || inode_num > sb->inode_count){
    return -EINVAL;
  }
  uint32_t inodes_per_block = sb->block_size / sizeof(inode_t);
  uint32_t block_num = sb->inode_table_block + (inode_num / inodes_per_block);
  uint32_t inode_offset = inode_num % inodes_per_block;

  uint8_t* buffer = (uint8_t*)kmalloc(sb->block_size);
  if (device->read(device, block_num, buffer, sb->block_size)< 0){
    kfree(buffer);
    return -EIO;
  }
  memcpy(buffer + inode_offset, inode, sizeof(inode_t));

  if (device->write(device, block_num, buffer, sb->block_size) < 0){
    kfree(buffer);
    return -EIO;
  }
  kfree(buffer);
  return 0;
}

int sfs_set_inode_bitmap(block_device_t *device, superblock_t *sb, uint32_t inode_num) {
  /*
  * Marks the inode as used in the inode bitmap.
  * Returns 0 on success, negative error code on failure.
  */

  if (inode_num == 0 || inode_num >= sb->inode_count) {
    return -EINVAL;
  }

  // Calculate which bitmap block contains this inode's bit
  uint32_t block_offset = inode_num / (sb->block_size * 8);
  uint32_t bit_in_block = inode_num % (sb->block_size * 8);

  // Read the bitmap block
  uint8_t* bitmap_block = (uint8_t*)kmalloc(sb->block_size);
  if (device->read(device, sb->inode_bitmap_block + block_offset, bitmap_block, sb->block_size) < 0) {
    kfree(bitmap_block);
    return -EIO;
  }

  // Set the bit
  bitmap_set(bitmap_block, bit_in_block);

  // Write back
  if (device->write(device, sb->inode_bitmap_block + block_offset, bitmap_block, sb->block_size) < 0) {
    kfree(bitmap_block);
    return -EIO;
  }

  kfree(bitmap_block);
  return 0;
}
