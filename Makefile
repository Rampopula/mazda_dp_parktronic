
######################################
# target
######################################
TARGET = mazda_dp_parktronic

######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og
# log verbosity
#	error = 0
#	info = 1
#	debug = 2
LOG_LEVEL = 2

#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
autogen/Core/Src/main.c \
autogen/Core/Src/stm32f1xx_it.c \
autogen/Core/Src/stm32f1xx_hal_msp.c \
autogen/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_can.c \
autogen/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi.c \
autogen/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c \
autogen/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c \
autogen/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c \
autogen/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c \
autogen/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c \
autogen/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
autogen/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
autogen/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
autogen/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c \
autogen/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
autogen/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c \
autogen/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c \
autogen/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c \
autogen/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c \
autogen/Core/Src/system_stm32f1xx.c \
common/time/time.c \
app/mdp.c \
app/beeper/beeper.c \
app/can_bus/can_bus.c \
app/can_bus/can_sniffer/can_sniffer.c \
app/can_bus/can_hal/can_hal.c \
app/can_bus/can_spi/can_spi.c \
app/can_bus/can_spi/mcp2515/mcp2515.c \
app/ptronic_decoder/falcon2616/falcon2616.c \
app/ptronic_decoder/falcon2616/falcon2616_gpio_intf.c

# ASM sources
ASM_SOURCES =  \
autogen/startup_stm32f103xb.s

#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m3

# fpu
# NONE for Cortex-M0/M0+/M3

# float-abi


# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS =

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32F103xB


# AS includes
AS_INCLUDES =

# C includes
C_INCLUDES =  \
-Iautogen/Core/Inc \
-Iautogen/Drivers/STM32F1xx_HAL_Driver/Inc \
-Iautogen/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy \
-Iautogen/Drivers/CMSIS/Device/ST/STM32F1xx/Include \
-Iautogen/Drivers/CMSIS/Include \
-Icommon/ \
-Icommon/time \
-Iboardinfo/ \
-Iapp/ \
-Iapp/misc/ \
-Iapp/beeper/ \
-Iapp/can_bus/ \
-Iapp/can_bus/can_sniffer \
-Iapp/can_bus/can_hal/ \
-Iapp/can_bus/can_spi/ \
-Iapp/can_bus/can_spi/mcp2515 \
-Iapp/ptronic_decoder/ \
-Iapp/ptronic_decoder/falcon2616

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections -DLOG_LEVEL=$(LOG_LEVEL)

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2 -DMDP_APP_DEBUG=1
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = autogen/STM32F103C8Tx_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
