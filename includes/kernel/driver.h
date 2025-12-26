#ifndef DRIVER_H
#define DRIVER_H

typedef struct driver_t {
    char name[32];
    void (*init)(void); 
    void (*exit)(void);
} driver_t;


void driver_register(driver_t* driver);
void driver_unregister(char* name);
driver_t* driver_find(char* name);

#endif