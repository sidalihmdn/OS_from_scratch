CXX = i686-elf-g++
CC  = i686-elf-gcc
GDB = i386-elf-gdb

CFLAGS   = -g -ffreestanding -O2 -Wall -Wextra
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti

C_SOURCES = $(wildcard drivers/*.cpp cpu/*.cpp libc/*.cpp kernel/*.cpp tests/*.cpp kernel/*/*.cpp)
ASM_SOURCES = $(wildcard cpu/*.asm kernel/mem/*.asm)

HEADERS = $(wildcard includes/*.h includes/*/*.h includes/*/*/*.h)

OBJ = $(patsubst %.cpp,%.o,$(C_SOURCES)) $(patsubst %.asm,%.o,$(ASM_SOURCES))

# Kernel binary
bin/kernel.bin: bin/kernel.elf
	i686-elf-objcopy -O binary $< $@

# ELF version for debugging
bin/kernel.elf: $(OBJ) bin/kernel_entry.o
	i686-elf-ld -o $@ -T linker.ld \
	bin/kernel_entry.o \
	$(filter-out bin/kernel_entry.o,$^)

bin/kernel_entry.o: kernel/kernel_entry.asm
	nasm $< -f elf -o $@

# Boot image
bin/os.bin: bin/boot.bin bin/kernel.bin
	cat $^ > $@
bin/boot.bin: boot/boot.asm
	mkdir -p bin
	nasm $< -f bin -o $@

# Compile C++
%.o: %.cpp $(HEADERS)
	$(CC) $(CXXFLAGS) -c $< -o $@

# Compile C
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble .asm → .o (ELF)
%.o: %.asm
	nasm $< -f elf -o $@

run: bin/os.bin
	qemu-system-i386 -fda bin/os.bin
	make clean

debug: bin/os.bin bin/kernel.elf
	qemu-system-i386 -s -S -fda bin/os.bin

clean:
	rm -rf $(OBJ) bin/*.bin bin/*.elf */*.o */*.o
