# ============================================
# Toolchain
# ============================================
CXX      := i686-elf-g++
CC       := i686-elf-gcc
AS       := nasm
LD       := i686-elf-ld
GDB      := i386-elf-gdb

# ============================================
# Directories
# ============================================
BUILD_DIR := build
OBJ_DIR   := $(BUILD_DIR)/obj
BIN_DIR   := $(BUILD_DIR)/bin
ISO_DIR   := $(BUILD_DIR)/iso

# ============================================
# Flags
# ============================================
WARNINGS := -Wall -Wextra -Werror=return-type
CFLAGS   := -g -ffreestanding -O2 $(WARNINGS) -Iincludes -MMD -MP
CXXFLAGS := $(CFLAGS) -fno-exceptions -fno-rtti -fno-use-cxa-atexit
ASFLAGS  := -f elf
LDFLAGS  := -T linker.ld

# ============================================
# Source Files
# ============================================
# Explicitly list source directories (more maintainable than wildcards)
C_DIRS := \
	arch/x86/cpu \
	arch/x86/interrupts \
	arch/x86/io \
	drivers/blocks/ata \
	drivers/blocks \
	kernel/driver \
	kernel/mem

CPP_DIRS := \
	arch/x86/time \
	drivers \
	drivers/clock \
	drivers/display \
	kernel \
	kernel/mem \
	cpu \
	libc \
	tests \
	boot

ASM_DIRS := \
	cpu \
	kernel/mem

# Find all source files
C_SOURCES   := $(foreach dir,$(C_DIRS),$(wildcard $(dir)/*.c))
CPP_SOURCES := $(foreach dir,$(CPP_DIRS),$(wildcard $(dir)/*.cpp))
ASM_SOURCES := $(foreach dir,$(ASM_DIRS),$(wildcard $(dir)/*.asm))

# Generate object file paths
C_OBJECTS   := $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
CPP_OBJECTS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(CPP_SOURCES))
ASM_OBJECTS := $(patsubst %.asm,$(OBJ_DIR)/%.o,$(ASM_SOURCES))

ALL_OBJECTS := $(C_OBJECTS) $(CPP_OBJECTS) $(ASM_OBJECTS) $(OBJ_DIR)/boot/multiboot.o

# Dependency files (for header tracking)
DEPS := $(ALL_OBJECTS:.o=.d)

# ============================================
# Targets
# ============================================
.PHONY: all clean iso run debug

all: $(BIN_DIR)/kernel.elf

# Link kernel
$(BIN_DIR)/kernel.elf: $(ALL_OBJECTS) | $(BIN_DIR)
	@echo "LD  $@"
	@$(LD) $(LDFLAGS) -o $@ $(ALL_OBJECTS)

# Compile C files
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	@echo "CXX  $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile C++ files
$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	@echo "CXX $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Assemble ASM files
$(OBJ_DIR)/%.o: %.asm | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	@echo "AS  $<"
	@$(AS) $(ASFLAGS) $< -o $@

# Create directories
$(BIN_DIR) $(OBJ_DIR):
	@mkdir -p $@

# Generate ISO
iso: $(BIN_DIR)/kernel.elf
	@echo "Creating ISO..."
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(BIN_DIR)/kernel.elf $(ISO_DIR)/boot/
	@echo 'set timeout=0' > $(ISO_DIR)/boot/grub/grub.cfg
	@echo 'set default=0' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo 'menuentry "OS from Scratch" {' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '    multiboot /boot/kernel.elf' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '    boot' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '}' >> $(ISO_DIR)/boot/grub/grub.cfg
	@i686-elf-grub-mkrescue -o os.iso $(ISO_DIR) 2>&1 | grep -v "xorriso"

# Run in QEMU
run: iso
	qemu-system-i386 -cdrom os.iso -hda hdd2.img

# Debug with GDB
debug: iso
	qemu-system-i386 -s -S -cdrom os.iso -hda hdd2.img &
	$(GDB) -ex "target remote localhost:1234" -ex "symbol-file $(BIN_DIR)/kernel.elf"

# Clean build artifacts
clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR) os.iso

# Include dependency files (for header tracking)
-include $(DEPS)

# Print build info
info:
	@echo "C sources:   $(words $(C_SOURCES))"
	@echo "C++ sources: $(words $(CPP_SOURCES))"
	@echo "ASM sources: $(words $(ASM_SOURCES))"
	@echo "Total:       $(words $(ALL_OBJECTS))"