#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>
#include "rob_gpio.h"

// --- REGISTERS ---
#define REG_OUT 0xA02
#define REG_IN  0xA03

// --- HARDWARE MAPPING STRUCT ---
typedef struct {
    int port_addr;  // 0xA02 or 0xA03
    int bit_num;    // 0-7
    int is_output;  // 1 = Output, 0 = Input
    int invert;     // 1 = Active Low (1 is OFF, 0 is ON), 0 = Normal
} pin_config_t;

// --- THE MAP ---
static const pin_config_t PIN_MAP[8] = {
    // INPUTS (DI1-DI4) - Usually Normal Logic (1=High Voltage)
    { REG_IN,  0, 0, 0 }, // DI1
    { REG_IN,  1, 0, 0 }, // DI2
    { REG_IN,  2, 0, 0 }, // DI3
    { REG_IN,  3, 0, 0 }, // DI4

    // OUTPUTS (DO1-DO4) - Active Low (Inverted)
    // We set 'invert' to 1 here. 
    // This means the library will flip the bit before writing.
    { REG_OUT, 0, 1, 1 }, // DO1
    { REG_OUT, 1, 1, 1 }, // DO2
    { REG_OUT, 2, 1, 1 }, // DO3
    { REG_OUT, 3, 1, 1 }  // DO4
};

// --- GLOBAL STATE TRACKING ---
// This array will always store the "Logical" state (HIGH=ON, LOW=OFF)
// It abstracts away the hardware inversion.
static int pin_states[8] = {0}; 

// --- SETUP ---
int rob_setup(void) {
    if (ioperm(REG_OUT, 2, 1)) {
        perror("[LIB-ROB] GPIO Init Failed.");
        return -1;
    }
    
    // Sync Logic: Read hardware, apply inversion map, store in array
    
    // Sync Outputs
    unsigned char out_reg = inb(REG_OUT);
    for(int i = 4; i <= 7; i++) {
        int physical_bit = (out_reg >> PIN_MAP[i].bit_num) & 1;
        // If inverted logic: Physical 0 -> Logical 1 (HIGH)
        if (PIN_MAP[i].invert) {
            pin_states[i] = !physical_bit; 
        } else {
            pin_states[i] = physical_bit;
        }
    }
    
    // Sync Inputs
    unsigned char in_reg = inb(REG_IN);
    for(int i = 0; i <= 3; i++) {
        int physical_bit = (in_reg >> PIN_MAP[i].bit_num) & 1;
        if (PIN_MAP[i].invert) {
            pin_states[i] = !physical_bit;
        } else {
            pin_states[i] = physical_bit;
        }
    }

    return 0;
}

// --- DIGITAL WRITE ---
void digitalWrite(int pin, int value) {
    if (pin < 0 || pin > 7) return;

    if (PIN_MAP[pin].is_output == 0) {
        fprintf(stderr, "[LIB-ROB] Error: Pin %d is Read-Only\n", pin);
        return;
    }

    // 1. Store the LOGICAL intent (e.g., User wants HIGH/ON)
    pin_states[pin] = (value == HIGH) ? HIGH : LOW;

    // 2. Determine PHYSICAL bit to write
    // If invert is true: HIGH -> 0, LOW -> 1
    int physical_value;
    if (PIN_MAP[pin].invert) {
        physical_value = (value == HIGH) ? 0 : 1; 
    } else {
        physical_value = (value == HIGH) ? 1 : 0;
    }

    // 3. Read-Modify-Write
    unsigned char current_reg = inb(PIN_MAP[pin].port_addr);
    unsigned char next_reg;
    int bit = PIN_MAP[pin].bit_num;

    if (physical_value == 1) {
        next_reg = current_reg | (1 << bit);
    } else {
        next_reg = current_reg & ~(1 << bit);
    }

    outb(next_reg, PIN_MAP[pin].port_addr);
}

// --- DIGITAL READ ---
int digitalRead(int pin) {
    if (pin < 0 || pin > 7) return -1;

    // If Output, return our logical memory state
    if (PIN_MAP[pin].is_output == 1) {
        return pin_states[pin];
    }

    // If Input, read hardware and map back to logical
    unsigned char reg_val = inb(PIN_MAP[pin].port_addr);
    int bit = PIN_MAP[pin].bit_num;
    int physical_val = (reg_val >> bit) & 1;
    
    int logical_val;
    if (PIN_MAP[pin].invert) {
        logical_val = !physical_val;
    } else {
        logical_val = physical_val;
    }
    
    pin_states[pin] = logical_val;
    return logical_val;
}

// --- DEBUG ---
void print_pin_states(void) {
    printf("--- ROB GPIO LOGICAL STATE (HIGH=ON) ---\n");
    printf("DI: [1:%d] [2:%d] [3:%d] [4:%d]\n", 
        pin_states[DI1], pin_states[DI2], pin_states[DI3], pin_states[DI4]);
    printf("DO: [1:%d] [2:%d] [3:%d] [4:%d]\n", 
        pin_states[DO1], pin_states[DO2], pin_states[DO3], pin_states[DO4]);
    printf("----------------------------------------\n");
}