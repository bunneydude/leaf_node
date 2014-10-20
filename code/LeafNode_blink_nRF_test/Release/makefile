################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: LPC810_CodeBase.axf

# Tool invocations
LPC810_CodeBase.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -Xlinker -Map="LPC810_CodeBase.map" -Xlinker --gc-sections -mcpu=cortex-m0 -mthumb -T "LPC810_CodeBase_Release.ld" -o "LPC810_CodeBase.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) LPC810_CodeBase.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "LPC810_CodeBase.axf"; arm-none-eabi-objcopy -O binary "LPC810_CodeBase.axf" "LPC810_CodeBase.bin" ; arm-none-eabi-objcopy -O ihex "LPC810_CodeBase.axf" "LPC810_CodeBase.hex" ; checksum -p LPC810 -d "LPC810_CodeBase.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
