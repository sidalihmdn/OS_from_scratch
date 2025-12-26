#include <arch/cpu_control.h>
#include <arch/x86/io/ports.h>
#include <arch/interrupt_controller.h>
#include <cpu/int.h>
#include <drivers/ata.h>
#include <drivers/block_device.h>
#include <kernel/driver.h>
#include <kernel/mem/heap.h>
#include <libc/log.h>
#include <libc/string.h>
#include <errno.h>
#include "ata_internals.h"
#include "consts.h"


ata_device_t* ata_get_device(uint8_t index);


static ata_device_t* devices;
static uint8_t ata_primary_irq;
static uint8_t ata_secondary_irq;

int ata_read_block(block_device_t* device, uint64_t lba, void* buffer, uint32_t size){
    return ata_read_lba28((ata_device_t*)device->data, lba, buffer, size);
}

int ata_write_block(block_device_t* device, uint64_t lba, void* buffer, uint32_t size){
    return ata_write_lba28((ata_device_t*)device->data, lba, buffer, size);
}

static void ata_init_device(ata_device_t* device, uint16_t base, uint16_t control, uint8_t slave){
    device->base = base;
    device->control = control;
    device->slave = slave;
    device->channel = 0;
    device->flags = 0;
    device->sector_size = 512;
    device->sector_count = 0;
    device->model[0] = 0;
}


void ata_init(){
    arch_disable_interrupts();
    devices = (ata_device_t*)kmalloc(sizeof(ata_device_t)*4);
    ata_init_device(&devices[0], ATA_PRIMARY_BASE, ATA_PRIMARY_CONTROL, ATA_MASTER_DRIVE);
    ata_init_device(&devices[1], ATA_PRIMARY_BASE, ATA_PRIMARY_CONTROL, ATA_SLAVE_DRIVE);
    ata_init_device(&devices[2], ATA_SECONDARY_BASE, ATA_SECONDARY_CONTROL, ATA_MASTER_DRIVE);
    ata_init_device(&devices[3], ATA_SECONDARY_BASE, ATA_SECONDARY_CONTROL, ATA_SLAVE_DRIVE);
    
    for (int i = 0; i < 4; i++){
        int result = ata_identify(&devices[i]);
        if (result < 0) {
            continue;
        }
        if (devices[i].flags & ATA_FLAG_PRESENT){
            block_device_t* block_device = (block_device_t*)kmalloc(sizeof(block_device_t));
            block_device->block_count = devices[i].sector_count;
            block_device->block_size = devices[i].sector_size;
            block_device->read = ata_read_block;
            block_device->write = ata_write_block;
            block_device->data = &devices[i];
            block_device_register(block_device);
        }
    }

    register_interrupt_handler(ATA_PRIMARY_IRQ + 32, ata_primary_irq_handler);
    register_interrupt_handler(ATA_SECONDARY_IRQ + 32, ata_secondary_irq_handler);
    ata_primary_irq = 0;
    ata_secondary_irq = 0;
    arch_enable_interrupts();
}

void ata_exit(){
    arch_disable_interrupts();
    kfree(devices);
    arch_enable_interrupts();
}

int ata_identify(ata_device_t* device){
    /*
    select the correct drive
    */
    ata_select_drive(device);

    /*
    floating bud check
    */
    if (ata_read_status(device) == 0xFF){
        return -EIO;
    }

    /*
    set the sector count, LBAlo, LBAmid, LBAhi registers to 0
    */
    outb(device->base + ATA_SEC_COUNT_REG, 0);
    outb(device->base + ATA_LBA_LOW_REG, 0);
    outb(device->base + ATA_LBA_MID_REG, 0);
    outb(device->base + ATA_LBA_HIGH_REG, 0);

    /*
    send the identify command
    */
    outb(device->base + ATA_COMMAND_REG, ATA_IDENRIFY_CMD);

    /*
    read status register to see if the drive exists
    */
    if (ata_read_status(device) == 0){
        return -EIO;
    }

    /*
    wait for the BSY bit to clear
    */
    while (ata_read_status(device) & ATA_STATUS_BSY){}
    
    /*
    check the LBAmid and LBAhi registers to see if they are 0
    */
    if (inb(device->base + ATA_LBA_MID_REG) != 0 || inb(device->base + ATA_LBA_HIGH_REG) != 0){
        /*
        the device is not a ATA device
        TODO : handle this case
        */
        return -EIO;
    }
    /*
    wait for the DRQ bit to set or error to set
    */
    while (!(ata_read_status(device) & ATA_STATUS_DRQ) && !(ata_read_status(device) & ATA_STATUS_ERR) ){}

    /*
    if error, return
    */
    if (ata_read_status(device) & ATA_STATUS_ERR){
        return -EIO;
    }

    /*
    read the identify data
    */
    uint16_t* buffer = (uint16_t*)kmalloc(sizeof(uint16_t)*256);
    for (int i = 0; i < 256; i++){
        buffer[i] = inw(device->base + ATA_RW_REG);
    }
    /*
    parse the identify data
    extract the model string
    extract the sector count
    extract the LBA48 support
    */
    for (int i = 0; i < 40; i+=2){
        device->model[i] = (buffer[27+(i/2)]>>8) & 0xFF;
        device->model[i+1] = buffer[27+(i/2)] & 0xFF;
    }
    device->model[40] = '\0';

    if (buffer[83] & (1<<10)){
        device->flags |= ATA_FLAG_LBA48;
        device->sector_count = 
            (uint64_t)buffer[100] |
            ((uint64_t)buffer[101]<<16) |
            ((uint64_t)buffer[102]<<32) |
            ((uint64_t)buffer[103]<<48);
    } else {
        device->flags |= ATA_FLAG_LBA28;
        device->sector_count = 
            (uint64_t)buffer[60] |
            ((uint64_t)buffer[61]<<16);
    }
    device->flags |= ATA_FLAG_PRESENT;
    LOG_F("ATA Drive Identified: %s, Sectors: %d (Size: %d MB)", 
          device->model, 
          device->sector_count, 
          (uint32_t)(device->sector_count * 512) / 1024 / 1024);
    kfree(buffer);
    return 0;
}

void ata_select_drive(ata_device_t* device){
    if (device->slave){
        outb(device->base + ATA_DRIVE_HEAD_REG, ATA_SELECT_SLAVE_DRIVE_CMD);
    }else{
        outb(device->base + ATA_DRIVE_HEAD_REG, ATA_SELECT_MASTER_DRIVE_CMD);
    }
}


uint8_t ata_read_status(ata_device_t* device){
    return inb(device->base + ATA_STATUS_REG);
}

ata_device_t* ata_get_device(uint8_t index){
    return &devices[index];
}

int ata_read_lba28(ata_device_t* device, uint64_t lba, void* buffer, uint32_t size){
    
    volatile uint8_t* irq_flag = NULL;
    uint32_t sector_count = (size+511)/512;

    if ( device == NULL || buffer == NULL ){
        return -EINVAL;
    }

    if (lba > 0x0FFFFFFF){
        return -EINVAL;
    }

    if (sector_count > 256){
        return -EINVAL;
    }

    /*
    select the correct drive
    */
    ata_select_drive(device);
    while (ata_read_status(device) & ATA_STATUS_BSY){}
    /*
    set the sector count, LBAlo, LBAmid, LBAhi registers
    */
    outb(device->base + ATA_DRIVE_HEAD_REG, 0xE0 | (device->slave<<4) | ((lba>>24)&0x0F));
    outb(device->base + ATA_SEC_COUNT_REG, sector_count);
    outb(device->base + ATA_LBA_LOW_REG, ((uint8_t)lba));
    outb(device->base + ATA_LBA_MID_REG, ((uint8_t)(lba >> 8)));
    outb(device->base + ATA_LBA_HIGH_REG, ((uint8_t)(lba >> 16)));

    /*
    set up the right irq flag
    */
    if (device->base == ATA_PRIMARY_BASE){
        irq_flag = &ata_primary_irq;
    }else{
        irq_flag = &ata_secondary_irq;
    }
    *irq_flag = 0;

    /*
    send the read command
    */
    outb(device->base + ATA_COMMAND_REG, ATA_READ_CMD);

    /*
    wait for the BSY bit to clear
    */
    while (1){
        uint8_t status = ata_read_status(device);
        if (status & ATA_STATUS_ERR) return -EIO;
        if (!(status & ATA_STATUS_BSY) && (status & ATA_STATUS_DRQ)){
            break;
        }
    }
    /*
    read the data
    */
    for (uint32_t i = 0; i < sector_count; i++){
        uint32_t timeout = 5000;
        while (*irq_flag == 0){
            arch_halt();
            if (--timeout == 0){
                return -EIO;
            }
        }
        *irq_flag = 0;
        for (uint32_t j = 0; j < 256; j++){
            ((uint16_t*)buffer)[i*256 + j] = inw(device->base + ATA_RW_REG);
        }
    }
    return 0;
}

int ata_write_lba28(ata_device_t* device, uint64_t lba, void* buffer, uint32_t size){
    uint32_t sector_count = (size+511)/512;
    uint16_t* sector_buffer = (uint16_t*)buffer;
    /*
    select the correct drive
    */
    ata_select_drive(device);
    while (ata_read_status(device) & ATA_STATUS_BSY){}
    /*
    set the sector count, LBAlo, LBAmid, LBAhi registers
    */
    outb(device->base + ATA_DRIVE_HEAD_REG, 0xE0 | (device->slave<<4) | ((lba>>24)&0x0F));
    outb(device->base + ATA_SEC_COUNT_REG, sector_count);
    outb(device->base + ATA_LBA_LOW_REG, ((uint8_t)lba));
    outb(device->base + ATA_LBA_MID_REG, ((uint8_t)(lba >> 8)));
    outb(device->base + ATA_LBA_HIGH_REG, ((uint8_t)(lba >> 16)));

    /*
    set up the right irq flag
    */
    volatile uint8_t* irq_flag = NULL;
    if (device->base == ATA_PRIMARY_BASE){
        irq_flag = &ata_primary_irq;
    }else if (device->base == ATA_SECONDARY_BASE){
        irq_flag = &ata_secondary_irq;
    }else{
        return -EINVAL;
    }
    *irq_flag = 0;

    /*
    send the Write command
    */
    outb(device->base + ATA_COMMAND_REG, ATA_WRITE_CMD);
    /*
    write the data
    */
    for (uint32_t i = 0; i < sector_count; i++){
        /*
        wait for the DRQ bit to set
        */
        while (1){
            uint8_t status = ata_read_status(device);
            if (status & ATA_STATUS_ERR) return -EIO;
            if (!(status & ATA_STATUS_BSY) && (status & ATA_STATUS_DRQ)){
                break;
            }
        }
        
        for (uint32_t j = 0; j < 256; j++){
            outw(device->base + ATA_RW_REG, sector_buffer[i*256+j]);
        }
        while (*irq_flag == 0){
            arch_halt();
        }
        *irq_flag = 0;
    }
    /*
    flush cache
    */
    outb(device->base + ATA_COMMAND_REG, 0xE7);
    while (*irq_flag == 0){
        arch_halt();
    }
    *irq_flag = 0;
    return 0;
}

void ata_primary_irq_handler(registers_t){
    inb(ATA_PRIMARY_BASE + ATA_STATUS_REG);
    ata_primary_irq = 1;
    arch_irq_send_eoi(ATA_PRIMARY_IRQ + 32);
}

void ata_secondary_irq_handler(registers_t){
    inb(ATA_SECONDARY_BASE + ATA_STATUS_REG);
    ata_secondary_irq = 1;
    arch_irq_send_eoi(ATA_SECONDARY_IRQ + 32);
}

driver_t ata_driver = {
    .name = "ATA PIO",
    .init = ata_init,
    .exit = ata_exit
};