#include <multiboot.h>
// Compares two strings lexicographically. 
// The comparison is based on the Unicode value of each character in the strings.
// Returns an integer less than, equal to, or greater than zero if str1 is found, 
// respectively, to be less than, to match, or be greater than str2.
int kstrcmp(const char* str1, const char* str2);

// Converts an integer value to a null-terminated string using the specified base 
// and stores the result in the array given by str parameter.
// If the base is 10 and value is negative, the resulting string is preceded with a minus sign (-).
// With any other base, value is always considered unsigned.
char* itoa(int num, char* str, int base);

void print_kernel_size(const multiboot_info_t* mbi);