#include <drivers/block_device.h>
#include <libc/log.h>

#define MAX_BLOCK_DEVICES 16

static block_device_t* devices[MAX_BLOCK_DEVICES];
static uint8_t device_count = 0;

void block_device_register(block_device_t* device){
    if (device_count >= MAX_BLOCK_DEVICES){
        LOG_ERROR("Block device register failed: too many devices");
        return;
    }
    devices[device_count] = device;
    device_count++;
}

block_device_t* block_device_get(uint8_t index){
    if (index >= device_count){
        LOG_ERROR("Block device get failed: invalid index");
        return NULL;
    }
    return devices[index];
}

uint8_t block_device_count(){
    return device_count;
}