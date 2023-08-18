COMPILER=i686-elf-gcc
ASSEMBLER=nasm
EMU=qemu-system-i386
FS_UTIL=./mtfs.out
RESULT_NAME=SimpleBoot
LOG=log.txt

DIR_FIRST_BOOTLDR=../first-bootloader
DIR_SECOND_BOOTLDR=../second-bootloader
DIR_THIRD_BOOTLDR=../third-bootloader
DIR_KERNEL16=../kernel16
DIR_KERNEL32=../kernel32
DIR_RESULT=bin
DIR_LIBRARY=../library

COMMON_COMPILE_FLAGS=-O0 -ffreestanding -Wall -Wextra
COMMON_LINK_FLAGS=-O0 -ffreestanding -Wall -Wextra -nostdlib -Wall -Wextra
SECOND_BOOTLDR_COMPILE_FLAGS=-m16 $(COMMON_COMPILE_FLAGS)
SECOND_BOOTLDR_LINK_FLAGS=-m16 $(COMMON_LINK_FLAGS) -T second-bootldr.ld

KERNEL16_COMPILE_FLAGS=-m16 $(COMMON_COMPILE_FLAGS) -I $(DIR_LIBRARY)
KERNEL16_LINK_FLAGS=-m16 $(COMMON_LINK_FLAGS) -T kernel16.ld -lgcc
KERNEL32_COMPILE_FLAGS=-m32 $(COMMON_COMPILE_FLAGS) -I $(DIR_LIBRARY)
KERNEL32_LINK_FLAGS=-m32 $(COMMON_LINK_FLAGS) -T kernel32.ld -lgcc

SECOND_BOOTLDR_C_SRCS=$(shell find $(DIR_SECOND_BOOTLDR)/ -name *.c)
SECOND_BOOTLDR_ASM_SRCS=$(shell find $(DIR_SECOND_BOOTLDR)/ -name *.asm)
SECOND_BOOTLDR_C_OBJS=$(addsuffix .2nd.o, $(SECOND_BOOTLDR_C_SRCS))
SECOND_BOOTLDR_ASM_OBJS=$(addsuffix .2nd.o, $(SECOND_BOOTLDR_ASM_SRCS))

KERNEL16_C_SRCS=$(shell find $(DIR_KERNEL16)/ -name *.c)
KERNEL16_ASM_SRCS=$(shell find $(DIR_KERNEL16)/ -name *.asm)
KERNEL16_ASM_OBJS=$(addsuffix .k16.o, $(KERNEL16_ASM_SRCS))
KERNEL16_C_OBJS=$(addsuffix .k16.o, $(KERNEL16_C_SRCS))

KERNEL32_C_SRCS=$(shell find $(DIR_KERNEL32)/ -name *.c)
KERNEL32_ASM_SRCS=$(shell find $(DIR_KERNEL32)/ -name *.asm)
KERNEL32_ASM_OBJS=$(addsuffix .k32.o, $(KERNEL32_ASM_SRCS))
KERNEL32_C_OBJS=$(addsuffix .k32.o, $(KERNEL32_C_SRCS))

LIBRARY_C_SRCS=$(shell find $(DIR_LIBRARY)/ -name *.c)
LIBRARY16_C_OBJS=$(addsuffix .lib16.o, $(LIBRARY_C_SRCS))
LIBRARY32_C_OBJS=$(addsuffix .lib32.o, $(LIBRARY_C_SRCS))

#	If you only want to build the kernel and remove the object files, then do this (change arguments as needed):
#	Real Mode [16-bit] kernel:
#		make build-kernel-cfg cpum=16 vmt=2 vmw=320 vmh=200 vmbpp=8 vma=0 && make build-os16 && make clear-objs
#	Protected Mode [32-bit] kernel:
#		make build-kernel-cfg cpum=32 vmt=3 vmw=800 vmh=600 vmbpp=24 vma=144 && make build-os32 && make clear-objs

all:	run-os32

#	Build examples:
test32:
	make build-kernel-cfg cpum=32 vmt=3 vmw=800 vmh=600 vmbpp=24 vma=144 && make build-os32 && make run-os32 && make clear-os32

test16:
	make build-kernel-cfg cpum=16 vmt=2 vmw=320 vmh=200 vmbpp=8 vma=0 && make build-os16 && make run-os16 && make clear-os16

os16:
	$(MAKE) build-os16
	$(MAKE) run-os16
	$(MAKE) clear-os16

os32:
	$(MAKE) build-os32
	$(MAKE) run-os32
	$(MAKE) clear-os32

build-os16:
	make build-kernel16
	make build-bootloader16

build-os32:
	make build-kernel32
	make build-bootloader32

clear-os16:
	$(MAKE) clear-kernel16
	$(MAKE) clear-bootloader
	rm -f $(DIR_RESULT)/$(RESULT_NAME)16.bin

clear-os32:
	$(MAKE) clear-kernel32
	$(MAKE) clear-bootloader
	rm -f $(DIR_RESULT)/$(RESULT_NAME)32.bin

clear-objs:
	rm -f $(SECOND_BOOTLDR_C_OBJS) $(SECOND_BOOTLDR_ASM_OBJS)
	rm -f $(KERNEL16_C_OBJS) $(KERNEL16_ASM_OBJS)
	rm -f $(KERNEL32_C_OBJS) $(KERNEL32_ASM_OBJS)
	rm -f $(LIBRARY16_C_OBJS) $(LIBRARY32_C_OBJS)

run-os16:
	$(EMU) -monitor stdio -m 256M -cpu max -drive format=raw,file=$(DIR_RESULT)/$(RESULT_NAME)16.bin

run-os32:
	$(EMU) -monitor stdio -m 256M -cpu max -drive format=raw,file=$(DIR_RESULT)/$(RESULT_NAME)32.bin

dbg-os16:
	$(EMU) -monitor stdio -m 256M -cpu max -drive format=raw,file=$(DIR_RESULT)/$(RESULT_NAME)16.bin -D $(DIR_RESULT)/$(LOG) -d cpu_reset

dbg-os32:
	$(EMU) -monitor stdio -m 256M -cpu max -drive format=raw,file=$(DIR_RESULT)/$(RESULT_NAME)32.bin -D $(DIR_RESULT)/$(LOG) -d cpu_resetd

#	Kernel boot config
build-kernel-cfg:
	rm -f ../first-bootloader/kernel-boot-cfg.asm

ifeq ($(vmt),)
	$(error video mode type (vmt) not set!)
endif
ifeq ($(cpum),)
	$(error target cpu mode (cpum) not set!)
endif
ifeq ($(vmw),)
	$(error video mode width (vmw) not set!)
endif
ifeq ($(vmh),)
	$(error video mode height (vmh) not set!)
endif
ifeq ($(vmbpp),)
	$(error video mode bits/pixel (vmbpp) not set!)
endif
ifeq ($(vma),)
	$(error video mode attributes (vma) not set!)
endif

	echo \
	"CPUM:\tdw $(cpum)\r\n.VMT:\tdw $(vmt)\r\n.VMW:\tdw $(vmw)\r\n.VMH:\tdw $(vmh)\r\n.VMBPP:\tdw $(vmbpp)\r\n.VMA:\tdw $(vma)"\
	"" > ../first-bootloader/kernel-boot-cfg.asm

#	First bootloader
build-first-bootloader16:
# $(MAKE) build-fs
	$(ASSEMBLER) -fbin $(DIR_FIRST_BOOTLDR)/main.asm -i $(DIR_FIRST_BOOTLDR)/ -o $(DIR_RESULT)/first-bootloader-main.bin
	$(ASSEMBLER) -fbin $(DIR_FIRST_BOOTLDR)/setup16.asm -I $(DIR_RESULT)/ -o $(DIR_RESULT)/first-bootloader.bin

build-first-bootloader32:
# $(MAKE) build-fs
	$(ASSEMBLER) -fbin $(DIR_FIRST_BOOTLDR)/main.asm -i $(DIR_FIRST_BOOTLDR)/ -o $(DIR_RESULT)/first-bootloader-main.bin
	$(ASSEMBLER) -fbin $(DIR_FIRST_BOOTLDR)/setup32.asm -I $(DIR_RESULT)/ -o $(DIR_RESULT)/first-bootloader.bin

clear-first-bootloader:
#	$(MAKE) clear-fs
	rm -f \
		$(DIR_RESULT)/first-bootloader.bin \
		$(DIR_RESULT)/first-bootloader-main.bin \
		$(DIR_RESULT)/first-bootloader-setup.bin \
		$(DIR_RESULT)/$(LOG)

#	Second bootloader
second-bootloader:
	$(MAKE) build-second-bootloader
	$(MAKE) run-second-bootloader

build-second-bootloader:
	$(MAKE) $(SECOND_BOOTLDR_C_OBJS)
	$(MAKE) $(SECOND_BOOTLDR_ASM_OBJS)
	$(COMPILER) $(SECOND_BOOTLDR_C_OBJS) $(SECOND_BOOTLDR_ASM_OBJS) $(SECOND_BOOTLDR_LINK_FLAGS) -o $(DIR_RESULT)/second-bootloader.bin

%.c.2nd.o:	%.c
	$(COMPILER) -c $< $(SECOND_BOOTLDR_COMPILE_FLAGS) -o $@

%.asm.2nd.o:	%.asm
	$(ASSEMBLER) -felf $< -o $@

clear-second-bootloader:
	rm -f $(DIR_RESULT)/second-bootloader.bin $(SECOND_BOOTLDR_C_OBJS) $(SECOND_BOOTLDR_ASM_OBJS)

#	Third bootloader
build-third-bootloader:
	$(ASSEMBLER) -fbin $(DIR_THIRD_BOOTLDR)/main.asm -o $(DIR_RESULT)/third-bootloader.bin

clear-third-bootloader:
	rm -f $(DIR_RESULT)/third-bootloader.bin

build-bootloader:
	$(MAKE) build-third-bootloader
	$(MAKE) build-second-bootloader
	$(MAKE) build-first-bootloader

clear-bootloader:
	$(MAKE) clear-first-bootloader
	$(MAKE) clear-second-bootloader
	$(MAKE) clear-third-bootloader

#	Bootloader
build-bootloader16:
	$(MAKE) build-third-bootloader
	$(MAKE) build-second-bootloader
	$(MAKE) build-first-bootloader16
	mv $(DIR_RESULT)/first-bootloader.bin $(DIR_RESULT)/$(RESULT_NAME)16.bin

build-bootloader32:
	$(MAKE) build-third-bootloader
	$(MAKE) build-second-bootloader
	$(MAKE) build-first-bootloader32
	mv $(DIR_RESULT)/first-bootloader.bin $(DIR_RESULT)/$(RESULT_NAME)32.bin

#	Kernel:
build-kernel16:
ifneq ($(LIBRARY_C_SRCS),)
	$(MAKE) $(LIBRARY16_C_OBJS)
endif
	$(MAKE) $(KERNEL16_C_OBJS)
ifneq ($(KERNEL16_ASM_SRCS),)
	$(MAKE) $(KERNEL16_ASM_OBJS)
endif
	$(COMPILER) $(LIBRARY16_C_OBJS) $(KERNEL16_ASM_OBJS) $(KERNEL16_C_OBJS) $(KERNEL16_LINK_FLAGS) -o $(DIR_RESULT)/kernel16.bin

build-kernel32:
ifneq ($(LIBRARY_C_SRCS),)
	$(MAKE) $(LIBRARY32_C_OBJS)
endif
	$(MAKE) $(KERNEL32_C_OBJS)
ifneq ($(KERNEL32_ASM_SRCS),)
	$(MAKE) $(KERNEL32_ASM_OBJS)
endif
	$(COMPILER) $(LIBRARY32_C_OBJS) $(KERNEL32_ASM_OBJS) $(KERNEL32_C_OBJS) $(KERNEL32_LINK_FLAGS) -o $(DIR_RESULT)/kernel32.bin

%.c.k16.o:		%.c
	$(COMPILER) -c $< $(KERNEL16_COMPILE_FLAGS) -o $@

%.c.k32.o:		%.c
	$(COMPILER) -c $< $(KERNEL32_COMPILE_FLAGS) -o $@

%.asm.k16.o:	%.asm
	$(ASSEMBLER) -felf $< -i $(DIR_KERNEL16)/ -o $@

%.asm.k32.o:	%.asm
	$(ASSEMBLER) -felf $< -i $(DIR_KERNEL32)/ -o $@

%.c.lib16.o:	%.c
	$(COMPILER) -c $< $(KERNEL16_COMPILE_FLAGS) -o $@

%.c.lib32.o:	%.c
	$(COMPILER) -c $< $(KERNEL32_COMPILE_FLAGS) -o $@

clear-kernel16:
	rm -f $(LIBRARY16_C_OBJS) $(KERNEL16_C_OBJS) $(KERNEL16_ASM_OBJS) $(DIR_RESULT)/kernel16.bin

clear-kernel32:
	rm -f $(LIBRARY32_C_OBJS) $(KERNEL32_C_OBJS) $(KERNEL32_ASM_OBJS) $(DIR_RESULT)/kernel32.bin