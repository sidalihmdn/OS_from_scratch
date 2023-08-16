# this file will build the bin/os.bin file 
# i don't know anything about make files, but i'll see how to write one
# Meanwhile i'll use this shell script

mkdir temp

# compile the kernel.cpp file
echo "###### compiling the kernel.cpp file ######"
i386-elf-gcc -ffreestanding -m32 -g -c "kernel/kernel.cpp" -o "temp/kernel.o"

# compiling the asm files
echo "###### compiling the asm files ######"

nasm "boot/boot.asm" -f bin -o "temp/boot.bin"
nasm "kernel/kernel_entry.asm" -f elf -o "temp/kernel_entry.o"
nasm "boot/disk_error.asm" -f bin -o "temp/disk_error.bin"
# linking the kernel entry withe the kernel code file

echo "###### Linking the files ######"
i386-elf-ld -o "temp/full_kernel.bin" -Ttext 0x1000 "temp/kernel_entry.o" "temp/kernel.o" --oformat binary

# concatinating the files into OS.bin
cat temp/boot.bin temp/full_kernel.bin > temp/full_os.bin
cat temp/full_os.bin temp/disk_error.bin > bin/OS.bin

rm -r temp