#ifndef ATA_INTERNALS_H
#define ATA_INTERNALS_H
#include <unit_types.h> 

typedef struct ata_device {
    uint16_t base;
    uint16_t control;
    uint8_t  slave;
    uint8_t  channel;

    uint8_t  flags;

    uint32_t sector_size;
    uint64_t sector_count;

    char model[41];
} ata_device_t;

 int ata_read_block(block_device_t* device, uint64_t lba, void* buffer, uint32_t size);
int ata_write_block(block_device_t* device, uint64_t lba, void* buffer, uint32_t size);
int ata_read_lba28(ata_device_t* device, uint64_t lba, void* buffer, uint32_t size);
int ata_write_lba28(ata_device_t* device, uint64_t lba, void* buffer, uint32_t size);
static void ata_select_drive(ata_device_t* device);
static uint8_t ata_read_status(ata_device_t* device);
static int ata_identify(ata_device_t* device);
void ata_primary_irq_handler(registers_t regs);
void ata_secondary_irq_handler(registers_t regs);

#endif