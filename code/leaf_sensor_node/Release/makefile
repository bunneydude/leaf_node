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
all: leaf_sensor_node.axf

# Tool invocations
leaf_sensor_node.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -Xlinker -Map="leaf_sensor_node.map" -Xlinker --gc-sections -mcpu=cortex-m0 -mthumb -T "LeafNode_blink_nRF_test_Release.ld" -o "leaf_sensor_node.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) leaf_sensor_node.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "leaf_sensor_node.axf"; arm-none-eabi-objcopy -O binary "leaf_sensor_node.axf" "leaf_sensor_node.bin" ; arm-none-eabi-objcopy -O ihex "leaf_sensor_node.axf" "leaf_sensor_node.hex" ; checksum -p LPC812 -d "leaf_sensor_node.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
