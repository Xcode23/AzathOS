#include <stdint.h>

extern "C" void higher_half_setup();
extern uintptr_t boot_page_directory;
extern uintptr_t boot_page_table1;
extern uintptr_t _kernel_end;
extern uintptr_t _kernel_start;
extern uintptr_t _text_end;
extern uintptr_t _text_start;
extern uintptr_t _rodata_end;
extern uintptr_t _rodata_start;

void higher_half_setup() {
  uintptr_t* page_directory = reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(&boot_page_directory) - 0xC0000000);
  uintptr_t* page_table = reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(&boot_page_table1) - 0xC0000000);
  uintptr_t kernel_start = reinterpret_cast<uintptr_t>(&_kernel_start);
  uintptr_t kernel_end = reinterpret_cast<uintptr_t>(&_kernel_end)  - 0xC0000000;
  uintptr_t text_start = reinterpret_cast<uintptr_t>(&_text_start)  - 0xC0000000;
  uintptr_t text_end = reinterpret_cast<uintptr_t>(&_text_end)  - 0xC0000000;
  uintptr_t rodata_start = reinterpret_cast<uintptr_t>(&_rodata_start)  - 0xC0000000;
  uintptr_t rodata_end = reinterpret_cast<uintptr_t>(&_rodata_end)  - 0xC0000000;

  for (uintptr_t page_frame = 0, i = 0; page_frame < kernel_end; page_frame += 4096, i++) {
    if (page_frame >= kernel_start) {
      page_table[i] = page_frame | 0x1; // set bit 0 for present
      if ((page_frame < text_start || page_frame > text_end) && (page_frame < rodata_start || page_frame > rodata_end)) {
        page_table[i] |= 0x2; // set bit 1 for writeable in memory except rodata and text areas
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