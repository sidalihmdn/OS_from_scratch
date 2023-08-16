
# building the kernel.of file from the kernel.cpp
temp/kernel.o : kernel/kernel.cpp
	i386-elf-gcc -ffreestanding -m32 -g -c "kernel/kernel.cpp" -o "temp/kernel.o"

temp/boot.bin : boot/boot.asm
	nasm "boot/boot.asm" -f bin -o "temp/boot.bin"

temp/kernel_entry.o : kernel/kernel_entry.asm
	nasm "kernel/kernel_entry.asm" -f elf -o "temp/kernel_entry.o"

temp/disk_error.bin : boot/disk_error.asm
	nasm "boot/disk_error.asm" -f bin -o "temp/disk_error.bin"

temp/full_kernel.bin : temp/kernel_entry.o temp/kernel.o
	i386-elf-ld -o "temp/full_kernel.bin" -Ttext 0x7ef0 "temp/kernel_entry.o" "temp/kernel.o" --oformat binary

temp/full_os.bin : temp/boot.bin temp/full_kernel.bin
	cat temp/boot.bin temp/full_kernel.bin > temp/full_os.bin

bin/OS.bin : temp/full_os.bin temp/disk_error.bin
	cat temp/full_os.bin temp/disk_error.bin > bin/OS.bin

