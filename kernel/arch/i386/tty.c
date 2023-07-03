#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>

#include "vga.h"

#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64


static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;


void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}


void terminal_setcursor(int row, int col) {
    unsigned short position = (row * VGA_WIDTH) + col;

    // cursor LOW port to VGA INDEX register
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position & 0xFF));
    // cursor HIGH port to VGA INDEX register
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((position >> 8) & 0xFF));
}

void terminal_movecursor(size_t x, size_t y) {
    // Calculate the 1D position from the 2D coordinates
    uint16_t pos = y * VGA_WIDTH + x;

    // Send a command to the VGA control register to tell it we are setting the cursor low byte
    outb(0x3D4, 0x0F);
    // Send the low byte of the cursor position to the VGA data register
    outb(0x3D5, (uint8_t) (pos & 0xFF));

    // Do the same for the cursor high byte
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void terminal_scroll(void) {
    uint16_t blank = vga_entry(' ', terminal_color);

    if (terminal_row >= VGA_HEIGHT) {
        // Move all lines up one
        for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
            for (size_t x = 0; x < VGA_WIDTH; x++) {
                terminal_buffer[y * VGA_WIDTH + x] = terminal_buffer[(y + 1) * VGA_WIDTH + x];
            }
        }

        // Clear the last line
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = blank;
        }

        terminal_row--;
    }
}


void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
    unsigned char uc = c;

    switch (uc) {
    case '\n': // Newline
        terminal_row++;
        terminal_column = 0;
        terminal_scroll();
        break;
    case '\r': // Carriage return
        terminal_column = 0;
        break;
    default: // Regular character
        terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
        if (++terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT) {
                terminal_row = 0;
            }
        }
        terminal_scroll(); 
        break;
    }

    // Always update cursor position
    terminal_movecursor(terminal_column, terminal_row);
}


void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++) {
		terminal_putchar(data[i]);
	}
	terminal_setcursor(terminal_row, terminal_column);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}

static char scancode_ascii_map[128] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x',
    'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0,
    // Function keys
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Numeric keypad
    '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',
    // Other keys
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    // fill the rest as needed
};
static char shifted_scancode_ascii_map[128] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|', 'Z', 'X',
    'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0,
    // Function keys
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // Numeric keypad
    '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',
    // Other keys
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void terminal_write_hex(uint8_t num) {
    char hexDigits[] = "0123456789ABCDEF";

    terminal_putchar('0');
    terminal_putchar('x');
    terminal_putchar(hexDigits[(num >> 4) & 0xF]);
    terminal_putchar(hexDigits[num & 0xF]);
}

char scancode_to_ascii(uint8_t scancode) {
    static bool shift_pressed = false;

    // Shift key press and release scancodes
    uint8_t left_shift_press = 0x2A;
    uint8_t right_shift_press = 0x36;
    uint8_t left_shift_release = 0xAA;
    uint8_t right_shift_release = 0xB6;

    // Update shift_pressed status
    if (scancode == left_shift_press || scancode == right_shift_press) {
        shift_pressed = true;
        return 0;
    } else if (scancode == left_shift_release || scancode == right_shift_release) {
        shift_pressed = false;
        return 0;
    }

    /* Make sure the scan code is within our array */
    if (scancode > 127) {
        return 0;
    }

    if (shift_pressed) {
        return shifted_scancode_ascii_map[scancode];
    } else {
        return scancode_ascii_map[scancode];
    }
}


/* Read scan code from the keyboard */
uint8_t read_scancode() {
    return inb(KEYBOARD_DATA_PORT);
}

/* Check if there's data waiting in the keyboard controller */
int is_data_ready() {
    return inb(KEYBOARD_STATUS_PORT) & 1;
}

char terminal_getchar() {
    char c = 0;
    
    while (1) {
        while (!is_data_ready()) {
            /* Do nothing */
        }
        uint8_t scancode = read_scancode();
        c = scancode_to_ascii(scancode);

        // Check if the highest bit is set (i.e., this is a release scancode)
        if (scancode & 0x80) {
            // Check if it's a shift release scancode
            if (scancode == 0xAA || scancode == 0xB6) {
                // Call the scancode_to_ascii function to update the shift_pressed flag
                scancode_to_ascii(scancode);
            }
            continue;
        }
        // Handle newline and carriage return, break the loop
        if (c == '\n' || c == '\r') {
            break;
        }
        // Ignore other non-printable characters
        if (c >= ' ') {
            break;
        }
    }
    return c;
}


void terminal_write_uint32(uint32_t num) {
    char str[11]; // uint32_t can be up to 10 digits in base 10 + null terminator
    int i = sizeof(str) - 1;

    str[i--] = '\0'; // null terminate the string
    if (num == 0) {
        str[i] = '0';
    } else {
        while (num > 0) {
            str[i--] = (num % 10) + '0';
            num /= 10;
        }
    }

    terminal_writestring(&str[i+1]);
}
