#include <stdio.h>
#include <kernel/tty.h>
#include <kernel/shell.h>

void kernel_main() {
	terminal_initialize();
	shell();
}

