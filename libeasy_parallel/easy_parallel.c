#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>
#include <unistd.h>
#include <pci/pci.h>
#include "easy_parallel.h"

// --- Internal Prototypes ---
static int ep_init(uint16_t address);
static void ep_digitalWrite(int pin, int state);
static int ep_digitalRead(int pin);
static uint16_t ep_detectAddress(void);
static void ep_close(void);

// --- Global Instance ---
EasyParallel_t DB25 = {
    .base_addr = 0,
    .shadow_data = 0,
    .shadow_control = 0,
    .init = ep_init,
    .digitalWrite = ep_digitalWrite,
    .digitalRead = ep_digitalRead,
    .detectAddress = ep_detectAddress,
    .close = ep_close
};

// --- Implementation ---

static uint16_t ep_detectAddress(void) {
    struct pci_access *pacc;
    struct pci_dev *dev;
    uint16_t found_addr = 0;

    pacc = pci_alloc();
    pci_init(pacc);
    pci_scan_bus(pacc);

    for (dev = pacc->devices; dev; dev = dev->next) {
        pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_CLASS);
        // Look for Parallel Controller (0701)
        if (dev->device_class == 0x0701) {
            for (int i = 0; i < 6; i++) {
                pciaddr_t addr = dev->base_addr[i];
                if (addr == 0) continue;
                if (addr & PCI_BASE_ADDRESS_SPACE_IO) {
                    found_addr = (uint16_t)(addr & PCI_BASE_ADDRESS_IO_MASK);
                    // Found a PCIe card (likely Andross or Falco)
                    goto cleanup; 
                }
            }
        }
    }

cleanup:
    pci_cleanup(pacc);
    
    // Fallback for Panther (Dell 9020) native port
    if (found_addr == 0) {
        // We can't strictly "detect" legacy 0x378 safely without risking a crash,
        // but we can return it as a suggestion if no PCIe card is found.
        return 0x378; 
    }
    
    return found_addr;
}

static int ep_init(uint16_t address) {
    if (address == 0) return -1;
    DB25.base_addr = address;

    // Request access to hardware
    if (ioperm(DB25.base_addr, 3, 1)) {
        perror("[EasyParallel] Init Failed (Root required)");
        return -1;
    }

    // Initialize Shadows
    DB25.shadow_data = 0x00; 
    DB25.shadow_control = 0x00; // Be careful, this pulls Strobe/AutoLF low
    outb(DB25.shadow_data, DB25.base_addr + 0); 
    
    return 0;
}

static void ep_digitalWrite(int pin, int state) {
    // Data Pins 2-9
    if (pin >= 2 && pin <= 9) {
        int bit = pin - 2;
        if (state == HIGH) DB25.shadow_data |= (1 << bit);
        else DB25.shadow_data &= ~(1 << bit);
        outb(DB25.shadow_data, DB25.base_addr + 0);
    }
    // Control Pins
    else {
        uint8_t bit_mask = 0;
        int invert = 0;
        // Map pins to bits
        switch(pin) {
            case 1:  bit_mask = 0x01; invert = 1; break;
            case 14: bit_mask = 0x02; invert = 1; break;
            case 16: bit_mask = 0x04; invert = 0; break;
            case 17: bit_mask = 0x08; invert = 1; break;
            default: return; 
        }
        int write_val = (invert) ? !state : state;
        if (write_val) DB25.shadow_control |= bit_mask;
        else DB25.shadow_control &= ~bit_mask;
        outb(DB25.shadow_control, DB25.base_addr + 2);
    }
}

static int ep_digitalRead(int pin) {
    if (pin >= 2 && pin <= 9) {
        return (DB25.shadow_data >> (pin - 2)) & 1;
    }
    uint8_t status_reg = inb(DB25.base_addr + 1);
    switch(pin) {
        case 10: return (status_reg >> 6) & 1;
        case 11: return !((status_reg >> 7) & 1); // Busy Inverted
        case 12: return (status_reg >> 5) & 1;
        case 13: return (status_reg >> 4) & 1;
        case 15: return (status_reg >> 3) & 1;
    }
    return LOW; 
}

static void ep_close(void) {
    if (DB25.base_addr != 0) ioperm(DB25.base_addr, 3, 0);
}