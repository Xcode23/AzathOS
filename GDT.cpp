#include <stdint.h>

#include "GDT.hpp"

namespace gdt {
    const uint8_t GDT_SIZE = 3;

    struct GDTEntry {
        uint16_t limit_1;           // segment limit bits 0..15
        uint16_t base_1;            // segment base bits 0..15
        uint8_t base_2;             // segment base bits 16..23
        uint8_t access;             // access byte
        uint8_t flags_and_limit2;   // segment limit bits 16..19 followed by special flags
        uint8_t base_3;              // segment base bits 24..31
    } __attribute__((packed));

    struct GDTDescriptor {
    	uint16_t size;
    	uint32_t address;
    } __attribute__((packed));

    GDTEntry gdt[GDT_SIZE];

    GDTDescriptor gdt_descriptor = {sizeof(GDTEntry) * GDT_SIZE - 1, reinterpret_cast<uint32_t>(&gdt)}; //TODO: Add comments

    void setup_gdt() {
        gdt[0] = {0, 0, 0, 0, 0, 0}; //first gdt entry should always be null
        gdt[1] = {                   //gdt entry for kernel code segment
            .limit_1 = 0xFFFF,
            .base_1 = 0x0,
            .base_2 = 0x0,
            .access = 0b10011010,           // flags bits 41 for RX if code, 43 for code, 44 for code and data, and 47 for valid sector
            .flags_and_limit2 = 0b11001111, //lower 4 bits are limit, higher 4 bits are flags 54 for protected mode, and 55 for segments bound to 4k
            .base_3 = 0x0
        };
        gdt[2] = {                   //gdt entry for kernel data segment
            .limit_1 = 0xFFFF,
            .base_1 = 0x0,
            .base_2 = 0x0,
            .access = 0b10010010,           //flags bits 41 for RW if data, 44 for code and data, and 47 for valid sector
            .flags_and_limit2 = 0b11001111, //lower 4 bits are limit, higher 4 bits are flags 54 for protected mode, and 55 for segments bound to 4k
            .base_3 = 0x0
        };
    }

    void load_gdt() { //TODO: explain
        asm volatile("lgdtl %0         \n\t"
                 "ljmp $0x08,$.L%= \n\t"
                 ".L%=:            \n\t"
                 "mov %1, %%ds     \n\t"
                 "mov %1, %%es     \n\t"
                 "mov %1, %%fs     \n\t"
                 "mov %1, %%gs     \n\t"
                 "mov %1, %%ss     \n\t" :: "m"(gdt_descriptor), "r"(0x10) );
    }
}