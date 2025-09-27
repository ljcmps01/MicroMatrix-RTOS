cmake_minimum_required(VERSION 3.15)
project(blinky C ASM)

set(STM32_HAL_DRIVER_PATH "drivers")

set(HAL_SRC
    ${STM32_HAL_DRIVER_PATH}/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal.c
    ${STM32_HAL_DRIVER_PATH}/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rcc.c
    ${STM32_HAL_DRIVER_PATH}/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_gpio.c
    ${STM32_HAL_DRIVER_PATH}/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_exti.c
    ${STM32_HAL_DRIVER_PATH}/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pwr.c
    ${STM32_HAL_DRIVER_PATH}/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_cortex.c
    ${STM32_HAL_DRIVER_PATH}/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rtc.c
    ${STM32_HAL_DRIVER_PATH}/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rtc_ex.c
    ${STM32_HAL_DRIVER_PATH}/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_tim.c
    ${STM32_HAL_DRIVER_PATH}/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_uart.c
)

# MCU settings
set(MCU_FLAGS     
    -mcpu=cortex-m0
    -mthumb
    -O2
    -g
)
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/linker/STM32F051K8Tx_FLASH.ld)

# Sources
file(GLOB SRC_FILES
    src/*.c
    FreeRTOS/*.c
    FreeRTOS/portable/GCC/ARM_CM0/*.c
    FreeRTOS/portable/MemMang/heap_4.c
    ${HAL_SRC}
    ${STM32_HAL_DRIVER_PATH}/CMSIS/Device/system_stm32f0xx.c
    src/SEGGER_RTT.c 
    src/SEGGER_RTT_printfk.c 

)

set(ASM_STARTUP startup/startup_stm32f051x8.s)

add_executable(${PROJECT_NAME}.elf ${SRC_FILES} ${ASM_STARTUP})

# Includes
target_include_directories(${PROJECT_NAME}.elf PRIVATE
    include
    src
    FreeRTOS/include
    FreeRTOS/portable/GCC/ARM_CM0
    ${STM32_HAL_DRIVER_PATH}/CMSIS/Core/Include
    ${STM32_HAL_DRIVER_PATH}/CMSIS/Device/Include
    ${STM32_HAL_DRIVER_PATH}/STM32F0xx_HAL_Driver/Inc
)

# Compiler / linker flags
target_compile_options(${PROJECT_NAME}.elf PRIVATE ${MCU_FLAGS})
target_link_options(${PROJECT_NAME}.elf PRIVATE
    ${MCU_FLAGS}
    -T${LINKER_SCRIPT}
    -Wl,--gc-sections
    -Wl,-Map=${PROJECT_NAME}.map
)

# Convert to binary + hex
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMAND arm-none-eabi-objcopy -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
)
