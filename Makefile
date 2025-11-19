###################################
#           PROJECT NAME          
###################################
PROJECT 	:= main
MAIN_SRC	:= application/main.c

###################################
#           ARCHITECTURE          
###################################
MCU         := cortex-m4
FPU         := -mfpu=fpv4-sp-d16
FLOAT_ABI   := -mfloat-abi=hard


###################################
#        STARTUP & LINKER          
###################################
LD_SCRIPT    := stm32f4_flash.ld
STARTUP_FILE := startup_stm32f40xx.s


###################################
#           TOOLCHAINS            
###################################
TOOLCHAINS_PATH   := /home/bdang/toolchains/gcc-arm-none-eabi-14.3
PREFIX_TOOLCHAINS := arm-none-eabi
GCC               := $(PREFIX_TOOLCHAINS)-gcc
CP                := $(PREFIX_TOOLCHAINS)-objcopy
ASM               := $(PREFIX_TOOLCHAINS)-as


####################################
#           DIRECTORIES
####################################
APP_INC_DIR      := application/inc 
APP_SRC_DIR      := application/src

MIDDLE_INC_DIR	 := middleware/inc
MIDDLE_SRC_DIR	 := middleware/src

CMSIS_INC_DIR    := hardware/CMSIS/inc

DEVICE_INC_DIR   := hardware/Device/inc
DEVICE_SRC_DIR   := hardware/Device/src

SPL_INC_DIR      := hardware/SPL/inc
SPL_SRC_DIR      := hardware/SPL/src

BUILD_DIR        := build
APP_BUILD_DIR    := build/application
MIDDLE_BUILD_DIR := build/middleware
DEVICE_BUILD_DIR := build/device
SPL_BUILD_DIR    := build/SPL

STARTUP_DIR      := hardware/Device/startup
LINKER_DIR       := hardware/Device/linker


####################################
#      SOURCES & OBJECTS LIST
####################################
APP_SRC_FILES    := $(wildcard $(APP_SRC_DIR)/*.c)
MIDDLE_SRC_FILES := $(wildcard $(MIDDLE_SRC_DIR)/*.c)
DEVICE_SRC_FILES := $(wildcard $(DEVICE_SRC_DIR)/*.c)
SPL_SRC_FILES    := $(wildcard $(SPL_SRC_DIR)/*.c)

APP_OBJECTS      := $(APP_SRC_FILES:$(APP_SRC_DIR)/%.c=$(APP_BUILD_DIR)/%.o)
MIDDLE_OBJECTS	 := $(MIDDLE_SRC_FILES:$(MIDDLE_SRC_DIR)/%.c=$(MIDDLE_BUILD_DIR)/%.o)
DEVICE_OBJECTS   := $(DEVICE_SRC_FILES:$(DEVICE_SRC_DIR)/%.c=$(DEVICE_BUILD_DIR)/%.o)
SPL_OBJECTS      := $(SPL_SRC_FILES:$(SPL_SRC_DIR)/%.c=$(SPL_BUILD_DIR)/%.o)
STARTUP_OBJECT   := $(BUILD_DIR)/startup/startup.o


####################################
#           OPT FLAGS
####################################
CFLAGS := -mcpu=$(MCU) -mthumb $(FPU) $(FLOAT_ABI) -Wall -O0 -g \
          -I$(CMSIS_INC_DIR) \
          -I$(DEVICE_INC_DIR) \
          -I$(APP_INC_DIR) \
		  -I$(MIDDLE_INC_DIR) \
          -I$(SPL_INC_DIR) \
          -DSTM32F40XX \
          -DUSE_STDPERIPH_DRIVER \
          -ffunction-sections -fdata-sections

LDFLAGS := -T$(LINKER_DIR)/$(LD_SCRIPT) \
           --specs=nano.specs \
           -Wl,--gc-sections \
           -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map \
           -Wl,--no-warn-rwx-segments
		   --specs=nosys.specs \


####################################
#           MAKE RULES
####################################
.PHONY: all clean flash

all: $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).bin $(BUILD_DIR)/$(PROJECT).hex

bin: $(BUILD_DIR)/$(PROJECT).bin
hex: $(BUILD_DIR)/$(PROJECT).hex

# Compile application source files
$(APP_BUILD_DIR)/%.o: $(APP_SRC_DIR)/%.c
	@mkdir -p $(APP_BUILD_DIR)
	$(TOOLCHAINS_PATH)/bin/$(GCC) $(CFLAGS) -c $< -o $@

# Compile middleware source files
$(MIDDLE_BUILD_DIR)/%.o: $(MIDDLE_SRC_DIR)/%.c
	@mkdir -p $(MIDDLE_BUILD_DIR)
	$(TOOLCHAINS_PATH)/bin/$(GCC) $(CFLAGS) -c $< -o $@

# Compile Device source files (system_stm32f4xx.c)
$(DEVICE_BUILD_DIR)/%.o: $(DEVICE_SRC_DIR)/%.c
	@mkdir -p $(DEVICE_BUILD_DIR)
	$(TOOLCHAINS_PATH)/bin/$(GCC) $(CFLAGS) -c $< -o $@

# Compile SPL source files
$(SPL_BUILD_DIR)/%.o: $(SPL_SRC_DIR)/%.c
	@mkdir -p $(SPL_BUILD_DIR)
	$(TOOLCHAINS_PATH)/bin/$(GCC) $(CFLAGS) -c $< -o $@

# Compile main.c
$(BUILD_DIR)/main.o: $(MAIN_SRC)
	@mkdir -p $(BUILD_DIR)
	$(TOOLCHAINS_PATH)/bin/$(GCC) $(CFLAGS) -c $< -o $@

# Assemble startup file
$(STARTUP_OBJECT): $(STARTUP_DIR)/$(STARTUP_FILE)
	@mkdir -p $(BUILD_DIR)/startup
	$(TOOLCHAINS_PATH)/bin/$(GCC) $(CFLAGS) -c $< -o $@

# Link to create ELF file
$(BUILD_DIR)/$(PROJECT).elf: $(BUILD_DIR)/main.o $(APP_OBJECTS) $(MIDDLE_OBJECTS) $(DEVICE_OBJECTS) $(SPL_OBJECTS) $(STARTUP_OBJECT)
	$(TOOLCHAINS_PATH)/bin/$(GCC) $(CFLAGS) $(LDFLAGS) $^ -o $@
	@echo "Build complete: $@"
	@$(TOOLCHAINS_PATH)/bin/arm-none-eabi-size $@

# Create BIN file
$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	$(TOOLCHAINS_PATH)/bin/$(CP) -O binary $< $@
	@echo "Created: $@"

# Create HEX file
$(BUILD_DIR)/$(PROJECT).hex: $(BUILD_DIR)/$(PROJECT).elf
	$(TOOLCHAINS_PATH)/bin/$(CP) -O ihex $< $@
	@echo "Created: $@"

# Clean build files
clean:
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete"

# Flash to STM32
flash: $(BUILD_DIR)/$(PROJECT).bin
	st-flash write $< 0x8000000
	@echo "Flash complete"

# Debug with GDB
debug: $(BUILD_DIR)/$(PROJECT).elf
	gdb-multiarch $< \
	-ex "target extended-remote localhost:3333" \
	-ex "monitor reset halt" \
	-ex "load" \
	-ex "break main" \
	-ex "continue"