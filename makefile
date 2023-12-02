.PHONY: all clean run build_project 

CC=/usr/local/i386elfgcc/bin/i386-elf-gcc
LD=/usr/local/i386elfgcc/bin/i386-elf-ld

LINKFLAGS=-T src/link.ld --oformat binary
CFLAGS=-ffreestanding -Wall -c -fpic -fno-pie -mno-red-zone

all: run

clean:
	wsl rm -rf build

run: build_project
	qemu-system-x86_64 -drive file=build/os.img,format=raw,index=0,media=disk

build_project: build build/bootloader build/kernel build/create_img.exe build/os.img
	

build: 
	wsl mkdir -p $@/

build/bootloader: build
	wsl mkdir -p $@/

build/kernel: build
	wsl mkdir -p $@/

CPPFLAGS=-std=c++20 -Wall -Wextra -Wpedantic -O2 
build/create_img.exe: create_img/main.cpp
	g++ $< ${CPPFLAGS} -o $@


build/os.img: build/bootloader/zero_sector.bin build/kernel.bin build/create_img.exe
	build/create_img.exe ./build/

build/bootloader/zero_sector.bin: src/bootloader/zero_sector.asm 
	nasm $< -f bin -o $@


build/bootloader/extended.obj: src/bootloader/extended.asm 
	nasm $< -f elf32 -o $@

build/kernel/kernel.obj: src/kernel/kernel.c
	wsl $(CC) $(CFLAGS) $< -o $@

build/kernel.bin: build/bootloader/extended.obj build/kernel/kernel.obj
	wsl $(LD) -o $@ $(LINKFLAGS) $^


