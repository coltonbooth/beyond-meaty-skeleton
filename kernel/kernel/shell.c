#include <stdio.h>
#include <stdint.h>
#include <kernel/tty.h>
#include <kernel/kernel_functions.h>
#include <kernel/sys_commands.h>
#include <multiboot.h>

#define MAX_COMMAND_LENGTH 256

void shell(const multiboot_info_t* mbi) {
    terminal_writestring("\nWelcome to the BSI Operating System, BSI/OS\n");
    print_kernel_size(mbi);
    terminal_writestring("Type 'hello' to see a greeting, 'quit' to exit.\n");
    terminal_writestring("\n\n\n");

    while (1) {  // Run until we break
        terminal_writestring("shell> "); // Command prompt

        char command[MAX_COMMAND_LENGTH];
        size_t command_length = 0;

        // Collect command
        while (1) {
            char c = terminal_getchar();  // Get user input

            // Handle newline and carriage return as the end of command
            if (c == '\n' || c == '\r') {
                terminal_putchar('\n');  // Echo the newline
                break;
            }

            // Ignore other special characters
            if (c < ' ') {
                continue;
            }

            if (command_length < MAX_COMMAND_LENGTH - 1) {
                // Add the character to the command and echo it back to the user
                command[command_length++] = c;
                terminal_putchar(c);
            }
        }

        // Don't forget to null-terminate the command
        command[command_length] = '\0';

        // Handle commands
        if (kstrcmp(command, "hello") == 0) {
            terminal_writestring("\nHello, world!\n");
        } else if (kstrcmp(command, "reset") == 0) {
            terminal_writestring("\nExiting shell...\n");
            reboot();
        } else if (kstrcmp(command, "basic") == 0) {
            terminal_writestring("\nStarting basic...\n");
        } else {
            terminal_writestring("\nCommand not recognized.\n");
        }
    }
}
