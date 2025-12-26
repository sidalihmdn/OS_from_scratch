#include <kernel/driver.h>
#include <libc/string.h>
#include <libc/log.h>
#include <unit_types.h>
#include "consts.h"


static driver_t* drivers[MAX_DRIVERS];
static uint8_t driver_count = 0;


void driver_register(driver_t* driver){
    if (driver_count >= MAX_DRIVERS) {
        LOG_ERROR("Driver register failed: too many drivers");
        return;
    }
    else{
        drivers[driver_count++] = driver;
        LOG_F("Driver registered: %s", driver->name);
    }
}

void driver_unregister(char* name){
    for (uint8_t i = 0; i < driver_count; i++){
        if (strcmp(drivers[i]->name, name) == 0){
            LOG_F("Driver unregistered: %s", name);
            drivers[i] = drivers[driver_count - 1];
            drivers[driver_count-1] = NULL;
            driver_count--;
            return;
        }
    }
    LOG_ERROR("Driver unregister failed: driver not found");
}

driver_t* driver_find(char* name){
    for (uint8_t i = 0; i < driver_count; i++){
        if (strcmp(drivers[i]->name, name) == 0){
            return drivers[i];
        }
    }
    LOG_ERROR("Driver find failed: driver not found");
    return NULL;
}

