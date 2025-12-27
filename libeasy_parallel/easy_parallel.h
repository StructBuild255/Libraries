#ifndef EASY_PARALLEL_H
#define EASY_PARALLEL_H

#include <stdint.h>

// Logic Levels
#define HIGH 1
#define LOW  0

// The Struct definition
typedef struct {
    uint16_t base_addr;
    
    uint8_t shadow_data;
    uint8_t shadow_control;

    // Function Pointers
    int (*init)(uint16_t address);
    void (*digitalWrite)(int pin, int state);
    int (*digitalRead)(int pin);
    uint16_t (*detectAddress)(void); // <-- New Feature
    void (*close)(void);
} EasyParallel_t;

extern EasyParallel_t DB25;

#endif