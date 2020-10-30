################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include rail-app-config/EFR32_B_2_2/gcc/subdir.mk
-include hal-efr32/subdir.mk
-include external-generated-files/subdir.mk
-include emlib/subdir.mk
-include emdrv/subdir.mk
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
all: pre-build main-build

# Main-build Target
main-build: toggle_tx.axf

# Tool invocations
toggle_tx.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m33 -mthumb -T "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//platform/base/hal/../../Device/SiliconLabs/EFR32BG22/Source/GCC/efr32bg22.ld" -L"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//platform/base/hal/micro/cortexm3/" -Xlinker --defsym=FLASH_SIZE=524288 -Xlinker --defsym=RAM_SIZE=32768 -Xlinker --defsym=FLASH_PAGE_SIZE=4096 -Xlinker --defsym=SIMEEPROM_SIZE=0 -Xlinker --defsym=PSSTORE_SIZE=0 -Xlinker --defsym=LONGTOKEN_SIZE=0 -Xlinker --defsym=HEADER_SIZE=512 -Xlinker --defsym=BTL_SIZE=24576 -Xlinker --defsym=LOCKBITS_IN_MAINFLASH_SIZE=8192 -Xlinker --defsym=APP_BTL=1 -Xlinker --defsym=EMBER_MALLOC_HEAP_SIZE=0 -Xlinker --gc-sections -Xlinker -Map="toggle_tx.map" -mfpu=fpv5-sp-d16 -mfloat-abi=hard --specs=nano.specs -o toggle_tx.axf -Wl,--start-group "./emdrv/dmadrv.o" "./emdrv/gpiointerrupt.o" "./emdrv/sl_sleeptimer.o" "./emdrv/sl_sleeptimer_hal_rtcc.o" "./emdrv/sleep.o" "./emdrv/tempdrv.o" "./emdrv/ustimer.o" "./emlib/em_adc.o" "./emlib/em_cmu.o" "./emlib/em_core.o" "./emlib/em_emu.o" "./emlib/em_eusart.o" "./emlib/em_gpio.o" "./emlib/em_i2c.o" "./emlib/em_ldma.o" "./emlib/em_leuart.o" "./emlib/em_msc.o" "./emlib/em_prs.o" "./emlib/em_rmu.o" "./emlib/em_rtcc.o" "./emlib/em_se.o" "./emlib/em_system.o" "./emlib/em_timer.o" "./emlib/em_usart.o" "./emlib/em_wdog.o" "./emlib/system_efr32bg22.o" "./external-generated-files/bootloader-interface-app.o" "./external-generated-files/bootloader-interface.o" "./external-generated-files/btl_interface.o" "./external-generated-files/btl_interface_storage.o" "./hal-efr32/bsp_init.o" "./hal-efr32/bsp_stk.o" "./hal-efr32/bsp_stk_leds.o" "./hal-efr32/dmactrl.o" "./hal-efr32/em_assert.o" "./hal-efr32/em_rtc.o" "./hal-efr32/hal_common.o" "./hal-efr32/hal_efr.o" "./hal-efr32/i2cspm.o" "./hal-efr32/mx25flash_spi.o" "./hal-efr32/retargetio.o" "./hal-efr32/retargetserial.o" "./hal-efr32/udelay.o" "./led_assert.o" "./main.o" "./rail_config.o" "./rx.o" "./tx.o" "./rail-app-config/EFR32_B_2_2/gcc/startup_efr32bg22.o" "C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7//platform/radio/rail_lib/autogen/librail_release/librail_efr32xg22_gcc_release.a" -lm -Wl,--end-group -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: toggle_tx.hex'
	arm-none-eabi-objcopy -O ihex "toggle_tx.axf" "toggle_tx.hex"
	@echo ' '

	@echo 'Building bin file: toggle_tx.bin'
	arm-none-eabi-objcopy -O binary "toggle_tx.axf" "toggle_tx.bin"
	@echo ' '

	@echo 'Building s37 file: toggle_tx.s37'
	arm-none-eabi-objcopy -O srec "toggle_tx.axf" "toggle_tx.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "toggle_tx.axf" -A
	@echo ' '

	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) toggle_tx.axf
	-@echo ' '

pre-build:
	-cmd //c 'C:\Users\jgamm\SimplicityStudio\v4_workspace\toggle_tx/simple_rail_with_hal-prebuild.bat' 'toggle_tx' 'C:/Users/jgamm/SimplicityStudio/v4_workspace/toggle_tx' '' 'C:\SiliconLabs\SimplicityStudio\v4'
	-@echo ' '

post-build:
	-cmd //c 'C:\Users\jgamm\SimplicityStudio\v4_workspace\toggle_tx/simple_rail_with_hal-postbuild.bat' 'toggle_tx' 'C:/Users/jgamm/SimplicityStudio/v4_workspace/toggle_tx' '' 'C:\SiliconLabs\SimplicityStudio\v4'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: main-build pre-build post-build

-include ../makefile.targets
