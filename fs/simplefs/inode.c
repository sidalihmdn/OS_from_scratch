#include <drivers/block_device.h>
#include <kernel/mem.h>
#include <libc/mem.h>
#include <errno.h>
#include "bitmap.h"
#include "inode.h"
#include "block.h"
#include "simplefs_internals.h"
#include "superblock.h"

uint32_t sfs_alloc_inode(superblock_t *sb) {
  if (sb->free_inodes == 0) {
    return 0; // No free inodes
  }

  uint32_t bitmap_block = sb->inode_bitmap_block;
  uint32_t bitmap_size = (sb->inode_count + 7) / 8;
  uint8_t* bitmap = (uint8_t*)kmalloc(bitmap_size);

  if (sb->device->read(sb->device, bitmap_block, bitmap, bitmap_size) < 0) {
    kfree(bitmap);
    return 0;
  }
  int inode_num = bitmap_alloc(bitmap, sb->inode_count);
  if (inode_num < 1) {
    kfree(bitmap);
    return 0;
  }

  if (sb->device->write(sb->device, bitmap_block, bitmap, bitmap_size) < 0) {
    kfree(bitmap);
    return 0;
  }
  sb->free_inodes--;
  if (sfs_write_superblock(sb) < 0) {
    kfree(bitmap);
    return 0; 
  }
  kfree(bitmap);
  return (uint32_t)inode_num;
}

void sfs_free_inode(superblock_t *sb, uint32_t inode_num) {
  uint32_t bitmap_block = sb->inode_bitmap_block;
  uint32_t bitmap_size = (sb->inode_count + 7) / 8;
  uint8_t* bitmap = (uint8_t*)kmalloc(bitmap_size);

  if (sb->device->read(sb->device, bitmap_block, bitmap, bitmap_size) < 0) {
    kfree(bitmap);
    return;
  }
  bitmap_free(bitmap, inode_num);

  if (sb->device->write(sb->device, bitmap_block, bitmap, bitmap_size) < 0) {
    kfree(bitmap);
    return;
  }
  sb->free_inodes++;
  sfs_write_superblock(sb);
  kfree(bitmap);
}

int sfs_read_inode(superblock_t *sb, uint32_t inode_num, inode_t *inode) {
  if (inode_num == 0 || inode_num > sb->inode_count){
    return -EINVAL;
  }
  uint32_t inodes_per_block = sb->block_size / sizeof(inode_t);
  uint32_t inode_offset = inode_num % inodes_per_block;
  uint32_t block_num = sb->inode_table_block + (inode_num / inodes_per_block);

  uint8_t* buffer = (uint8_t*)kmalloc(sb->block_size);
  if (sb->device->read(sb->device, block_num, buffer, sb->block_size) < 0){
    kfree(buffer);
    return -EIO;
  }

  memcpy(inode, buffer + inode_offset, sizeof(inode_t));
  kfree(buffer);
  return 0;
}

int sfs_write_inode(superblock_t *sb, uint32_t inode_num, inode_t *inode) {
  /* this function writes */
  
  if (inode_num == 0 || inode_num > sb->inode_count){
    return -EINVAL;
  }
  uint32_t inodes_per_block = sb->block_size / sizeof(inode_t);
  uint32_t block_num = sb->inode_table_block + (inode_num / inodes_per_block);
  uint32_t inode_offset = inode_num % inodes_per_block;

  uint8_t* buffer = (uint8_t*)kmalloc(sb->block_size);
  if (sb->device->read(sb->device, block_num, buffer, sb->block_size)< 0){
    kfree(buffer);
    return -EIO;
  }
  memcpy(buffer + inode_offset, inode, sizeof(inode_t));

  if (sb->device->write(sb->device, block_num, buffer, sb->block_size) < 0){
    kfree(buffer);
    return -EIO;
  }
  kfree(buffer);
  return 0;
}

int sfs_set_inode_bitmap(superblock_t *sb, uint32_t inode_num) {
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
  if (sb->device->read(sb->device, sb->inode_bitmap_block + block_offset, bitmap_block, sb->block_size) < 0) {
    kfree(bitmap_block);
    return -EIO;
  }

  // Set the bit
  bitmap_set(bitmap_block, bit_in_block);

  // Write back
  if (sb->device->write(sb->device, sb->inode_bitmap_block + block_offset, bitmap_block, sb->block_size) < 0) {
    kfree(bitmap_block);
    return -EIO;
  }

  kfree(bitmap_block);
  return 0;
}

int sfs_unmap_block(superblock_t* sb, inode_t* inode, uint32_t block_number){
  /* unmap the block from the inode  */

  /* check if it a direct block */
  if (block_number < SIMPLEFS_MAX_DIRECT){
    if (inode->blocks[block_number] != 0){
      inode->blocks[block_number] = 0;
      return 0;
    }
    return 0;
  }

  /* indirect block */
  if (inode->indirect == 0){
    return 0;
  }

  block_number -= SIMPLEFS_MAX_DIRECT;
  uint32_t ptr_per_block = SIMPLEFS_BLOCK_SIZE / sizeof(uint32_t);

  if (block_number > ptr_per_block){
    return -EINVAL;
  }
  
  uint32_t* indirect_ptrs = (uint32_t*)kmalloc(SIMPLEFS_BLOCK_SIZE);
  if (sfs_read_indirect_block_ptrs(sb, inode, indirect_ptrs) < 0){
    kfree(indirect_ptrs);
    return -EIO;
  }
  indirect_ptrs[block_number] = 0;


  /* check if the indirect block is empty */
  int all_zero = 1;
  for (uint32_t i = 0; i < ptr_per_block; i++){
    if (indirect_ptrs[i] != 0){
      all_zero = 0;
      break;
    }
  }
  if (all_zero){
    sfs_free_block(sb, inode->indirect);
    inode->indirect = 0;
  }else{
    if (sfs_write_indirect_block_ptrs(sb, inode, indirect_ptrs) < 0){
      kfree(indirect_ptrs);
      return -EIO;
    }
  }
  kfree(indirect_ptrs);
  return 0;
}