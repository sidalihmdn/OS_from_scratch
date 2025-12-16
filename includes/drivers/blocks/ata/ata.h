#ifndef ATA_H
#define ATA_H

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


void ata_read(uint64_t lba, void* buffer, uint32_t size);
void ata_write(uint64_t lba, void* buffer, uint32_t size);

void ata_init();

#endif