#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#define MULTIBOOT_INFO_MEMORY     0x00000001
#define MULTIBOOT_INFO_BOOTDEV    0x00000002
#define MULTIBOOT_INFO_CMDLINE    0x00000004
#define MULTIBOOT_INFO_MODS       0x00000008
#define MULTIBOOT_INFO_AOUT_SYMS  0x00000010
#define MULTIBOOT_INFO_ELF_SHDR   0X00000020
#define MULTIBOOT_INFO_MEM_MAP    0x00000040
#define MULTIBOOT_INFO_DRIVE_INFO 0x00000080
#define MULTIBOOT_INFO_CONFIG_TBL 0x00000100
#define MULTIBOOT_INFO_BOOT_LOADER_NAME 0x00000200
#define MULTIBOOT_INFO_APM_TABLE  0x00000400
#define MULTIBOOT_INFO_VBE_INFO   0x00000800
#define MULTIBOOT_INFO_FRAMEBUFFER_INFO 0x00001000

typedef unsigned short          multiboot_uint16_t;
typedef unsigned int            multiboot_uint32_t;

typedef unsigned long long      multiboot_uint64_t;

struct multiboot_info {
  multiboot_uint32_t flags;

  multiboot_uint32_t mem_lower;
  multiboot_uint32_t mem_upper;

  multiboot_uint32_t boot_device;

  multiboot_uint32_t cmdline;

  multiboot_uint32_t mods_count;
  multiboot_uint32_t mods_addr;

  struct {
    multiboot_uint32_t num;
    multiboot_uint32_t size;
    multiboot_uint32_t addr;
    multiboot_uint32_t shndx;
  } elf_sec;

  multiboot_uint32_t mmap_length;
  multiboot_uint32_t mmap_addr;

  multiboot_uint32_t drives_length;
  multiboot_uint32_t drives_addr;

  multiboot_uint32_t config_table;

  multiboot_uint32_t boot_loader_name;

  multiboot_uint32_t apm_table;

  multiboot_uint32_t vbe_control_info;
  multiboot_uint32_t vbe_mode_info;
  multiboot_uint16_t vbe_mode;
  multiboot_uint16_t vbe_interface_seg;
  multiboot_uint16_t vbe_interface_off;
  multiboot_uint16_t vbe_interface_len;
};


typedef struct multiboot_info multiboot_info_t;

#endif /* MULTIBOOT_H */
