################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6/hardware/kit/common/bsp/bsp_bcc.c \
/Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6/hardware/kit/common/bsp/bsp_stk.c \
/Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6/hardware/kit/common/bsp/bsp_stk_leds.c \
/Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6/hardware/kit/common/bsp/bsp_trace.c 

OBJS += \
./BSP/bsp_bcc.o \
./BSP/bsp_stk.o \
./BSP/bsp_stk_leds.o \
./BSP/bsp_trace.o 

C_DEPS += \
./BSP/bsp_bcc.d \
./BSP/bsp_stk.d \
./BSP/bsp_stk_leds.d \
./BSP/bsp_trace.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/bsp_bcc.o: /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6/hardware/kit/common/bsp/bsp_bcc.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//platform/micrium_os" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//app/mcu_example/SLSTK3402A_EFM32PG12/micriumos_blink/cfg" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//app/mcu_example/SLSTK3402A_EFM32PG12/micriumos_blink" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//util/third_party/segger/systemview/SEGGER" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//util/third_party/segger/systemview/Config" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//util/third_party/segger/systemview/Sample/MicriumOSKernel" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//platform/CMSIS/Include" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//platform/emlib/inc" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/bsp" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//platform/micrium_os/bsp/siliconlabs/generic/include" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/drivers" -I"/Users/andyhsu/SimplicityStudio/v4_workspace/RTOS_Lab3_Tasks/Drivers" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"BSP/bsp_bcc.d" -MT"BSP/bsp_bcc.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BSP/bsp_stk.o: /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6/hardware/kit/common/bsp/bsp_stk.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//platform/micrium_os" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//app/mcu_example/SLSTK3402A_EFM32PG12/micriumos_blink/cfg" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//app/mcu_example/SLSTK3402A_EFM32PG12/micriumos_blink" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//util/third_party/segger/systemview/SEGGER" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//util/third_party/segger/systemview/Config" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//util/third_party/segger/systemview/Sample/MicriumOSKernel" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//platform/CMSIS/Include" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//platform/emlib/inc" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/bsp" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//platform/micrium_os/bsp/siliconlabs/generic/include" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/drivers" -I"/Users/andyhsu/SimplicityStudio/v4_workspace/RTOS_Lab3_Tasks/Drivers" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"BSP/bsp_stk.d" -MT"BSP/bsp_stk.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BSP/bsp_stk_leds.o: /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6/hardware/kit/common/bsp/bsp_stk_leds.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//platform/micrium_os" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//app/mcu_example/SLSTK3402A_EFM32PG12/micriumos_blink/cfg" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//app/mcu_example/SLSTK3402A_EFM32PG12/micriumos_blink" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//util/third_party/segger/systemview/SEGGER" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//util/third_party/segger/systemview/Config" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//util/third_party/segger/systemview/Sample/MicriumOSKernel" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//platform/CMSIS/Include" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//platform/emlib/inc" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/bsp" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//platform/micrium_os/bsp/siliconlabs/generic/include" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/drivers" -I"/Users/andyhsu/SimplicityStudio/v4_workspace/RTOS_Lab3_Tasks/Drivers" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"BSP/bsp_stk_leds.d" -MT"BSP/bsp_stk_leds.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

BSP/bsp_trace.o: /Applications/Simplicity\ Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6/hardware/kit/common/bsp/bsp_trace.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//platform/micrium_os" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//app/mcu_example/SLSTK3402A_EFM32PG12/micriumos_blink/cfg" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//app/mcu_example/SLSTK3402A_EFM32PG12/micriumos_blink" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//util/third_party/segger/systemview/SEGGER" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//util/third_party/segger/systemview/Config" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//util/third_party/segger/systemview/Sample/MicriumOSKernel" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//platform/CMSIS/Include" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//platform/emlib/inc" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/bsp" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//platform/micrium_os/bsp/siliconlabs/generic/include" -I"/Applications/Simplicity Studio.app/Contents/Eclipse/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/drivers" -I"/Users/andyhsu/SimplicityStudio/v4_workspace/RTOS_Lab3_Tasks/Drivers" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"BSP/bsp_trace.d" -MT"BSP/bsp_trace.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


