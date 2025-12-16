#include <drivers/blocks/ata/ata.h>
#include <kernel/mem/heap.h>
#include <arch/x86/io/ports.h>
#include <arch/cpu_control.h>
#include <arch/interrupt_controller.h>
#include <cpu/int.h>
#include <libc/string.h>
#include <libc/log.h>

#define ATA_PRIMARY_BASE 0x1F0
#define ATA_SECONDARY_BASE 0x170


#define ATA_PRIMARY_CONTROL 0x3F6
#define ATA_SECONDARY_CONTROL 0x376

#define ATA_SLAVE_DRIVE 1
#define ATA_MASTER_DRIVE 0


/*
 * ATA Status Register
 */
#define ATA_STATUS_BSY 0x80
#define ATA_STATUS_DRDY 0x40
#define ATA_STATUS_DRQ 0x08
#define ATA_STATUS_ERR 0x01

/*
 * ATA PIO Registers offsets from IO base
 */
#define ATA_RW_REG  0x0
#define ATA_ERR_REG 0x1
#define ATA_FEATURE_REG 0x1
#define ATA_SEC_COUNT_REG 0x2
#define ATA_LBA_LOW_REG 0x3
#define ATA_LBA_MID_REG 0x4
#define ATA_LBA_HIGH_REG 0x5
#define ATA_DRIVE_HEAD_REG 0x6
#define ATA_STATUS_REG 0x7
#define ATA_COMMAND_REG 0x7

/*
 * ATA PIO Registers offsets from IO control base
 */
#define ATA_ALT_STATUS_REG 0x0
#define ATA_CONTROL_REG 0x0
#define ATA_DRIVE_ADDRESS_REG 0x1

/*
 * ATA Commands
 */
#define ATA_IDENRIFY_CMD 0xEC
#define ATA_SELECT_SLAVE_DRIVE_CMD 0xB0
#define ATA_SELECT_MASTER_DRIVE_CMD 0xA0
#define ATA_READ_CMD 0x20
#define ATA_WRITE_CMD 0x30
#define ATA_SELECT_LBA48_CMD 0x24

/*
flags
*/
#define ATA_FLAG_PRESENT 0x01
#define ATA_FLAG_LBA48 0x02
#define ATA_FLAG_LBA28 0x04

/*
 * ATA Interrupts
 */
#define ATA_PRIMARY_IRQ 14
#define ATA_SECONDARY_IRQ 15


/*
* ATA Functions
*/
ata_device_t* ata_get_device(uint8_t index);
void ata_read_lba28(ata_device_t* device, uint64_t lba, void* buffer, uint32_t size);
static void ata_select_drive(ata_device_t* device);
static uint8_t ata_read_status(ata_device_t* device);
static void ata_identify(ata_device_t* device);
static void ata_read_lba48(ata_device_t* device, uint64_t lba, void* buffer, uint32_t size);
void ata_primary_irq_handler(registers_t regs);
void ata_secondary_irq_handler(registers_t regs);

static ata_device_t* devices;
static uint8_t ata_primary_irq;
static uint8_t ata_secondary_irq;

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
    ata_identify(&devices[0]);
    ata_identify(&devices[1]);
    ata_identify(&devices[2]);
    ata_identify(&devices[3]);

    register_interrupt_handler(ATA_PRIMARY_IRQ + 32, ata_primary_irq_handler);
    register_interrupt_handler(ATA_SECONDARY_IRQ + 32, ata_secondary_irq_handler);
    ata_primary_irq = 0;
    ata_secondary_irq = 0;
    arch_enable_interrupts();
}

void ata_identify(ata_device_t* device){
    /*
    select the correct drive
    */
    ata_select_drive(device);

    /*
    floating bud check
    */
    if (ata_read_status(device) == 0xFF){
        LOG_ERROR("ATA Identify failed");
        return;
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
        return;
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
        return;
    }
    /*
    wait for the DRQ bit to set or error to set
    */
    while (!(ata_read_status(device) & ATA_STATUS_DRQ) && !(ata_read_status(device) & ATA_STATUS_ERR) ){}

    /*
    if error, return
    */
    if (ata_read_status(device) & ATA_STATUS_ERR){
        return;
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
    device->model[41] = '\0';

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

void ata_read_lba28(ata_device_t* device, uint64_t lba, void* buffer, uint32_t size){
    /*
    select the correct drive
    */
    ata_select_drive(device);
    while (ata_read_status(device) & ATA_STATUS_BSY){}
    LOG_F("ATA Read LBA28: %d", lba);
    /*
    set the sector count, LBAlo, LBAmid, LBAhi registers
    */
    outb(device->base + ATA_DRIVE_HEAD_REG, 0xE0 | (device->slave<<4) | ((lba>>24)&0x0F));
    outb(device->base + ATA_SEC_COUNT_REG, (size+511)/512);
    outb(device->base + ATA_LBA_LOW_REG, ((uint8_t)lba));
    outb(device->base + ATA_LBA_MID_REG, ((uint8_t)(lba >> 8)));
    outb(device->base + ATA_LBA_HIGH_REG, ((uint8_t)(lba >> 16)));

    /*
    set up the right irq flag
    */
    volatile uint8_t* irq_flag = NULL;
    if (device->base == ATA_PRIMARY_BASE){
        irq_flag = &ata_primary_irq;
    }else{
        irq_flag = &ata_secondary_irq;
    }
    *irq_flag = 0;

    LOG_F("ATA Read LBA28: %d", lba);
    /*
    send the read command
    */
    outb(device->base + ATA_COMMAND_REG, ATA_READ_CMD);

    /*
    wait for the BSY bit to clear
    */
    while (ata_read_status(device) & ATA_STATUS_BSY){}
    LOG_F("ATA Read LBA28: %d", lba);
    /*
    read the data
    */
    uint32_t sector_count = (size+511)/512;
    for (uint32_t i = 0; i < sector_count; i++){
        while (*irq_flag == 0){
            LOG_F("ATA Read LBA28: %d", lba);
            arch_halt();
        }
        *irq_flag = 0;
        for (uint32_t j = 0; j < 256; j++){
            ((uint16_t*)buffer)[i*256 + j] = inw(device->base + ATA_RW_REG);
        }
    }
}
void ata_write_lba28(ata_device_t* device, uint64_t lba, void* buffer, uint32_t size){
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
    }else{
        irq_flag = &ata_secondary_irq;
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
            if (status & ATA_STATUS_ERR) return;
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
}

void ata_primary_irq_handler(registers_t regs){
    LOG("ATA Primary IRQ");
    inb(ATA_PRIMARY_BASE + ATA_STATUS_REG);
    ata_primary_irq = 1;
    arch_irq_send_eoi(ATA_PRIMARY_IRQ + 32);
}

void ata_secondary_irq_handler(registers_t regs){
    LOG("ATA Secondary IRQ");
    inb(ATA_SECONDARY_BASE + ATA_STATUS_REG);
    ata_secondary_irq = 1;
    arch_irq_send_eoi(ATA_SECONDARY_IRQ + 32);
}