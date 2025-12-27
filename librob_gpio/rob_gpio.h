#ifndef ROB_GPIO_H
#define ROB_GPIO_H

// --- LOGIC STATES ---
#define HIGH 1
#define LOW  0

// --- PIN DEFINITIONS (MATCHING PHYSICAL LAYOUT) ---
// Left to Right on the case: DI1..4 then DO1..4
// We give them IDs 0 through 7 based on physical position
#define DI1 0
#define DI2 1
#define DI3 2
#define DI4 3

#define DO1 4
#define DO2 5
#define DO3 6
#define DO4 7

// --- FUNCTION PROTOTYPES ---
int rob_setup(void);
void digitalWrite(int pin, int value);
int digitalRead(int pin);
void print_pin_states(void);

#endif