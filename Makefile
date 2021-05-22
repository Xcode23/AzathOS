HOST=i686-elf
ARCH=i386

ARCHDIR=arch/$(ARCH)

OS_NAME=azathos

CC=$(HOST)-g++
AS=$(HOST)-as

CPPFLAGS=-ffreestanding -O2 -g -Wall -Wextra -fno-exceptions -fno-rtti -std=c++20 -Iinclude

KERNEL_FILE=$(OS_NAME).kernel
ISO_FILE=$(OS_NAME).iso

QEMU_FLAGS= --enable-kvm -cdrom $(ISO_FILE)
QEMU_COMMAND=qemu-system-$(ARCH)
QEMU=$(QEMU_COMMAND) $(QEMU_FLAGS)

CRTBEGIN_OBJ:=$(shell $(CC) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) -print-file-name=crtend.o)

KERNEL_ARCH_OBJS=$(ARCHDIR)/boot.o \
$(ARCHDIR)/tty.o
KERNEL_OBJS=$(KERNEL_ARCH_OBJS) \
kernel.o
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

.cpp.o:
	$(CC) -c $< -o $@ $(CPPFLAGS)

.s.o:
	$(AS) -c $< -o $@

run: $(ISO_FILE)
	$(QEMU)

clean:
	rm -rf *.o *.kernel *.iso isodir/
	rm -rf $(ARCHDIR)/*.o
