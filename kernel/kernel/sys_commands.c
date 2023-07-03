#include <stdint.h>
#include <kernel/tty.h>

void reboot() {
    uint8_t temp;

    asm volatile ("cli"); // disable all interrupts

    // Pulse the CPU's reset line
    do {
        temp = inb(0x64);  // Read from keyboard controller's command register
        if ((temp & 0x01) != 0) {
            inb(0x60);    // If bit 0 is set, read from input buffer to clear
            continue;
        }
    } while ((temp & 0x02) != 0);  // If bit 1 is set, wait until it's cleared

    // Pulse the reset line
    outb(0x64, 0xFE);

    asm volatile ("hlt"); // halt the CPU (just in case rebooting fails)
}