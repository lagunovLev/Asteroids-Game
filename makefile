.PHONY: all clean run build_project 

CC=/usr/local/i386elfgcc/bin/i386-elf-gcc
LD=/usr/local/i386elfgcc/bin/i386-elf-ld

LINKFLAGS=-T src/link.ld --oformat binary
CFLAGS=-ffreestanding -Wall -c -ggdb3 -fpic -fno-pie -mno-red-zone

all: run

clean:
	wsl rm -rf build

run: build_project
	qemu-system-x86_64 -debugcon stdio -drive file=build/os.img,format=raw,index=0,media=disk 

debug: build_project
	qemu-system-x86_64 -s -S -debugcon stdio -drive file=build/os.img,format=raw,index=0,media=disk 

folders_src:=$(wildcard src/*/) $(wildcard src/*/*/) $(wildcard src/*/*/*/) $(wildcard src/*/*/*/*/) 
folders_build:=$(patsubst src/%/,build/%/,$(folders_src))
build_project: $(folders_build) build/create_img.exe build/os.img

build/%/: 
	wsl mkdir -p $@

CPPFLAGS=-std=c++20 -Wall -Wextra -Wpedantic -O2 
build/create_img.exe: create_img/main.cpp
	g++ $< ${CPPFLAGS} -o $@


build/os.img: build/bootloader/zero_sector.bin build/kernel.bin build/create_img.exe
	build/create_img.exe ./build/

build/bootloader/zero_sector.bin: src/bootloader/zero_sector.asm 
	nasm $< -f bin -o $@

build/bootloader/extended.obj: src/bootloader/extended.asm 
	nasm $< -f elf32 -o $@


c_files:=$(wildcard src/kernel/*.c) $(wildcard src/kernel/*/*.c) $(wildcard src/kernel/*/*/*.c) $(wildcard src/kernel/*/*/*/*.c) 
asm_files:=$(wildcard src/kernel/*.asm) $(wildcard src/kernel/*/*.asm) $(wildcard src/kernel/*/*/*.asm) $(wildcard src/kernel/*/*/*/*.asm) 
obj_c_files:=$(patsubst src/kernel/%.c,build/kernel/%.obj,$(c_files))
obj_asm_files:=$(patsubst src/kernel/%.asm,build/kernel/%.obj,$(asm_files))

build/kernel/%.obj: src/kernel/%.c 
	wsl $(CC) $(CFLAGS) $< -o $@

build/kernel/%.obj: src/kernel/%.asm
	nasm $< -f elf32 -o $@

build/kernel.bin: build/bootloader/extended.obj $(obj_c_files) $(obj_asm_files)
	wsl $(LD) -o $@ $(LINKFLAGS) $^


