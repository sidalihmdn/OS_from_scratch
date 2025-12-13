CXX = i686-elf-g++
CC  = i686-elf-gcc
GDB = i386-elf-gdb

CFLAGS   = -g -ffreestanding -O2 -Wall -Wextra -Iincludes
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti

C_SOURCES = $(wildcard drivers/*.cpp drivers/*/*.cpp cpu/*.cpp libc/*.cpp kernel/*.cpp tests/*.cpp kernel/*/*.cpp boot/*.cpp)
ASM_SOURCES = $(wildcard cpu/*.asm kernel/mem/*.asm)

HEADERS = $(wildcard includes/*.h includes/*/*.h includes/*/*/*.h)

OBJ = $(patsubst %.cpp,%.o,$(C_SOURCES)) $(patsubst %.asm,%.o,$(ASM_SOURCES))

# Kernel ELF
bin/kernel.elf: bin/multiboot.o $(OBJ)
	i686-elf-ld -o $@ -T linker.ld bin/multiboot.o $(OBJ)

bin/multiboot.o: boot/multiboot.asm
	mkdir -p bin
	nasm $< -f elf -o $@

# ISO generation
iso: bin/kernel.elf
	mkdir -p build/iso/boot/grub
	cp bin/kernel.elf build/iso/boot/
	cp build/iso/boot/grub/grub.cfg build/iso/boot/grub/grub.cfg.bak 2>/dev/null || true
	echo 'set timeout=0' > build/iso/boot/grub/grub.cfg
	echo 'set default=0' >> build/iso/boot/grub/grub.cfg
	echo '' >> build/iso/boot/grub/grub.cfg
	echo 'menuentry "OS from Scratch" {' >> build/iso/boot/grub/grub.cfg
	echo '    multiboot /boot/kernel.elf' >> build/iso/boot/grub/grub.cfg
	echo '    boot' >> build/iso/boot/grub/grub.cfg
	echo '}' >> build/iso/boot/grub/grub.cfg
	i686-elf-grub-mkrescue -o os.iso build/iso

run: iso
	qemu-system-i386 -cdrom os.iso

# Compile C++
%.o: %.cpp $(HEADERS)
	$(CC) $(CXXFLAGS) -c $< -o $@

# Compile C
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble .asm → .o (ELF)
%.o: %.asm
	nasm $< -f elf -o $@

debug: bin/kernel.elf
	qemu-system-i386 -s -S -cdrom os.iso &
	$(GDB) -ex "target remote localhost:1234" -ex "symbol-file bin/kernel.elf"

clean:
	rm -rf $(OBJ) */*.bin */*.elf */*.o */*/*.o os.iso build/iso
