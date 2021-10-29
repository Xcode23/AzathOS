#include <stdint.h>
#include <stdbool.h>

#include "tty.hpp"
#include "interrupts.hpp"
#include "PIC.hpp"
#include "kernel.hpp"
#include "GDT.hpp"
#include "string.hpp"

struct multiboot_info
{
  /* Multiboot info version number */
  uint32_t flags;

  /* Available memory from BIOS */
  uint32_t mem_lower;
  uint32_t mem_upper;

  /* "root" partition */
  uint32_t boot_device;

  /* Kernel command line */
  uint32_t cmdline;

  /* Boot-Module list */
  uint32_t mods_count;
  uint32_t mods_addr;

  uint32_t padding1;
  uint32_t padding2;
  uint32_t padding3;
  uint32_t padding4;

  /* Memory Mapping buffer */
  uint32_t mmap_length;
  uint32_t mmap_addr;

  /* Drive Info buffer */
  uint32_t drives_length;
  uint32_t drives_addr;

  /* ROM configuration table */
  uint32_t config_table;

  /* Boot Loader Name */
  uint32_t boot_loader_name;

  /* APM table */
  uint32_t apm_table;

  /* Video */
  uint32_t vbe_control_info;
  uint32_t vbe_mode_info;
  uint16_t vbe_mode;
  uint16_t vbe_interface_seg;
  uint16_t vbe_interface_off;
  uint16_t vbe_interface_len;

  uint64_t framebuffer_addr;
  uint32_t framebuffer_pitch;
  uint32_t framebuffer_width;
  uint32_t framebuffer_height;
  uint8_t framebuffer_bpp;
#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED 0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB     1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT     2
  uint8_t framebuffer_type;
  union
  {
    struct
    {
      uint32_t framebuffer_palette_addr;
      uint16_t framebuffer_palette_num_colors;
    };
    struct
    {
      uint8_t framebuffer_red_field_position;
      uint8_t framebuffer_red_mask_size;
      uint8_t framebuffer_green_field_position;
      uint8_t framebuffer_green_mask_size;
      uint8_t framebuffer_blue_field_position;
      uint8_t framebuffer_blue_mask_size;
    };
  };
};
typedef struct multiboot_info multiboot_info_t;

struct multiboot_mmap_entry
{
  uint32_t size;
  uint32_t addr_low;
  uint32_t addr_high;
  uint32_t len_low;
  uint32_t len_high;
#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5
  uint32_t type;
} __attribute__((packed));
typedef struct multiboot_mmap_entry multiboot_memory_map_t;


extern "C" void kernel_main(/*multiboot_info_t* mbd*/);
extern "C" void higher_half_setup();
extern uintptr_t boot_page_directory;
extern uintptr_t boot_page_table1;
extern uintptr_t _kernel_end;
extern uintptr_t _kernel_start;
extern uintptr_t _text_end;
extern uintptr_t _text_start;
extern uintptr_t _rodata_end;
extern uintptr_t _rodata_start;
static bool is_protected();

static bool is_protected(){
	uint16_t status = 0;
	asm("smsw %0"
			:"=r"(status)
			:
			:);
	return status & 1;
}
 
void kernel_main(/*multiboot_info_t* mbd*/){
	gdt::setup_gdt();
	gdt::load_gdt();
	terminal_initialize();
	prints("Hello, kernel World!\n");

	if (is_protected())
		prints("The Kernel is in Protected Mode\n");
	else
		prints("The Kernel is in Real Mode\n");
	setup_idt();
	load_idt();
	pic_init();
	test_idt();
	asm("sti");

	while(1);
}

void panic() { //TODO: improve panic
	prints("PANIC");
	asm volatile("cli; hlt");
}

void higher_half_setup() {
  uintptr_t* page_directory = reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(&boot_page_directory) - 0xC0000000);
  uintptr_t* page_table = reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(&boot_page_table1) - 0xC0000000);
  uintptr_t kernel_start = reinterpret_cast<uintptr_t>(&_kernel_start);
  uintptr_t kernel_end = reinterpret_cast<uintptr_t>(&_kernel_end)  - 0xC0000000;
  uintptr_t text_start = reinterpret_cast<uintptr_t>(&_text_start)  - 0xC0000000;
  uintptr_t text_end = reinterpret_cast<uintptr_t>(&_text_end)  - 0xC0000000;
  uintptr_t rodata_start = reinterpret_cast<uintptr_t>(&_rodata_start)  - 0xC0000000;
  uintptr_t rodata_end = reinterpret_cast<uintptr_t>(&_rodata_end)  - 0xC0000000;
  // TODO: mark rodata and text as read only
  for (uintptr_t page_frame = 0, i = 0; page_frame < kernel_end; page_frame += 4096, i++) {
    if (page_frame >= kernel_start) {
      page_table[i] = page_frame | 0x1; // set bit 0 for present
      if ((page_frame < text_start || page_frame > text_end) && (page_frame < rodata_start || page_frame > rodata_end)) {
        page_table[i] |= 0x2;
      }
    }
  }

  // map 0x000B8000 with present and writable bits set to virtual memory
  page_table[1023] = 0x000B8000 | 0x3;

  // Map the page table to both virtual addresses 0x00000000 and 0xC0000000.
	page_directory[0] = (reinterpret_cast<uintptr_t>(&boot_page_table1) - 0xC0000000) | 0x3;
	page_directory[768] = (reinterpret_cast<uintptr_t>(&boot_page_table1) - 0xC0000000) | 0x3;

  // Set cr3 to the address of the boot_page_directory.
  asm volatile("mov %0, %%cr3" :: "r"(page_directory));

	// Enable paging and the write-protect bit.
  uintptr_t new_cr0;
  asm volatile("mov %%cr0, %0" : "=r"(new_cr0) : );
  new_cr0 |= 0x80010000;
	asm volatile("mov %0, %%cr0" :: "r"(new_cr0));
}