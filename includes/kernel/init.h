#ifndef KERNEL_INIT_H
#define KERNEL_INIT_H

#include <unit_types.h>
#include <boot/multiboot_helpers.h>

void init_interrupts();
void init_memory(multiboot_info_t* mb_info);


#endif