#include <stdint.h>
#include <stdio.h>
#include <multiboot.h>
#include <kernel/tty.h>


int kstrcmp(const char* str1, const char* str2) {
    while(*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}


char* itoa(int num, char* str, int base)
{
    int i = 0;
    int isNegative = 0;
  
    /* Handle 0 explicitely */
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
  
    // In standard itoa(), negative numbers are handled only with base 10.  
    // Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10) {
        isNegative = 1;
        num = -num;
    }
  
    // Process individual digits
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
  
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
  
    str[i] = '\0'; // Append string terminator
  
    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
  
    return str;
}

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

void print_kernel_size(const multiboot_info_t* mbi) {
    if (mbi->flags & MULTIBOOT_INFO_MEMORY) {

        // Compute the kernel size from the start and end symbols defined in the linker script
        uint32_t kernel_size = ((&_kernel_end - &_kernel_start) + 1023) / 1024; // Size in KB

        terminal_writestring("Kernel size: ");
        terminal_write_uint32(kernel_size);
        terminal_writestring("KB\n");
    } else {
        terminal_writestring("Could not determine memory size.\n");
    }
}
