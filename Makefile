C_SOURCES = $(wildcard io_functions/*.cpp kernel/*.cpp drivers/*.cpp)
HEADERS = $(wildcard io_functions/*.h kernel/*.h drivers/*.h */*.h)
# Nice syntax for file extension replacement
OBJ = $(patsubst %.cpp,%.o,$(C_SOURCES))

# Change this if your cross-compiler is somewhere else
CC =i386-elf-gcc
GDB =i386-elf-gdb
# -g: Use debugging symbols in gcc
CFLAGS = -g

# First rule is run by default
os.bin: boot/boot.bin kernel.bin
	cat $^ > os.bin

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
kernel.bin: boot/kernel_entry.o	int/int_asm.o ${OBJ}
	i386-elf-ld -o $@ -Ttext 0x1000 $^ --oformat binary

# Used for debugging purposes
kernel.elf: boot/kernel_entry.o ${OBJ}
	i386-elf-ld -o $@ -Ttext 0x1000 $^ 

run: os.bin
	qemu-system-i386 -fda os.bin

# Open the connection to qemu and load our kernel-object file with symbols
debug: os.bin kernel.elf
	qemu-system-i386 -s -fda os.bin &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.cpp ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os.bin *.elf
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o
