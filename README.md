# Simple i386 Operating System (x86 OS) ReadMe

Welcome to the **Simple i386 Operating System** project! This project aims to demonstrate the basics of creating a simple operating system for the x86 architecture. The kernel is written in C, while the boot loader is written in assembly language. This README will guide you through the project setup, building, and running the operating system on a compatible x86 machine or emulator.

## Project Structure

```
.
├── boot
│   ├── boot.asm     # Boot loader written in assembly
├── kernel
│   ├── kernel.cpp           # Kernel written in C
├── Makefile               # Build automation using Make
├── build.sh               # Build automation using shell script (deprecated)
├── README.md              # You are here
```

## Requirements

- A x86-based machine or emulator (QEMU is recommended for testing)
- GCC cross-compiler targeting i386 architecture
- NASM assembler

## Getting Started

1. **Clone the repository:**
   
   ```
   git clone https://github.com/yourusername/simple-i386-os.git
   cd simple-i386-os
   ```

2. **Build the Bootloader and Kernel:**

   - Run the following commands in the project root directory to build the bootloader and kernel:
   
     ```
     make all
     ```

   - This will generate a `bootloader.bin` and `kernel.bin` in their respective directories.

3. **Run the Operating System:**

   - You can use an x86 emulator like QEMU to test the operating system:
   
     ```
     qemu-system-i386 -drive format=raw,file=bin/OS.bin
     ```

   - This will launch the operating system in the emulator.

## Contributing

Contributions to this project are welcome! If you have any improvements, bug fixes, or new features, feel free to submit a pull request.

## License

This project is licensed under the [MIT License](LICENSE), which allows you to freely use, modify, and distribute the code for personal and commercial purposes.

## Acknowledgments

This project is inspired by various tutorials, online resources, and open-source operating system projects. 

## Disclaimer

This is a simple educational project and may not be suitable for production use. Use it to learn about operating system development and x86 architecture.


**HAMDANE Sid-ALi**
**hamdane.sidali@hotmail.com**
**Date: August 16, 2023**