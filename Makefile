HOST=i686-elf
ARCH=i386

ARCHDIR=arch/$(ARCH)

OS_NAME=azathos

CC=$(HOST)-g++
AS=nasm

CPPFLAGS=-ffreestanding -O2 -g -fno-exceptions -fno-rtti -std=c++20 -Iinclude $(WFLAGS)
WFLAGS= -Wall -Wextra -Wcast-align \
-Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization \
-Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations \
-Wmissing-include-dirs -Wnoexcept -Wold-style-cast \
-Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion \
-Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 \
-Wswitch-default -Wundef -Werror -Wno-unused

KERNEL_FILE=$(OS_NAME).kernel
ISO_FILE=$(OS_NAME).iso

QEMU_FLAGS= --enable-kvm -cdrom $(ISO_FILE)
QEMU_COMMAND=qemu-system-$(ARCH)
QEMU=$(QEMU_COMMAND) $(QEMU_FLAGS)

CRTBEGIN_OBJ:=$(shell $(CC) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) -print-file-name=crtend.o)

SRCFILES := $(shell find . -type f -name "*.cpp")
OBJFILES := $(patsubst %.cpp,%.o,$(SRCFILES))

KERNEL_OBJS=$(ARCHDIR)/boot.o $(OBJFILES)
OBJS=$(ARCHDIR)/crti.o \
$(KERNEL_OBJS) \
$(ARCHDIR)/crtn.o

.PHONY : run clean
.SUFFIXES : .o .cpp .s

$(ISO_FILE): $(KERNEL_FILE)
	mkdir -p isodir/boot/grub/
	cp grub.cfg isodir/boot/grub
	cp $(KERNEL_FILE) isodir/boot/
	grub-mkrescue -o $(ISO_FILE) isodir/

$(KERNEL_FILE): $(OBJS)
	$(CC) -T linker.ld -o $(KERNEL_FILE) -ffreestanding -O2 -nostdlib $(OBJS) -lgcc

%.o: %.cpp
	$(CC) -c $< -o $@ $(CPPFLAGS)

%.o: %.s
	$(AS) -felf32 $< -o $@

run: $(ISO_FILE)
	$(QEMU)

clean:
	rm -rf *.o *.kernel *.iso isodir/
	rm -rf $(ARCHDIR)/*.o
