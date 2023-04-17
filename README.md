# AzathOS

Hobby kernel made to learn how OS kernels are implemented.

## Roadmap

* Framebuffer based Text UI :heavy_check_mark:
* Keyboard Input :heavy_check_mark:
* Paging
* Memory management
* x86 extensions support (SSE, AVX, FPU, MMX)
* Klib data-structures: string, map, vector...
* Serial Output Logging
* Concurrency Protection (Mutexes, Semaphores, Spinlocks)
* Process management
* Filesystem
* Uefi support
* x86_64 support
* Arm support

## TODO

* move paging activation to c++
* allocate page for multiboot info
* handle the case where the kernel surpasses 3 MiB(dynamically generate page tables)
* refactor files/classes/namespaces/dependencies
* add privilege level to exceptions and hardware interrupts
* move keyboard driver logic out of the interrupt handler
* add documentation comments
* explain enable paging
* makefile move build files to build folder(use cmake?)