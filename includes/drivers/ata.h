#ifndef ATA_H
#define ATA_H

#include <kernel/driver.h>

void ata_init();
void ata_exit();

extern driver_t ata_driver;

#endif