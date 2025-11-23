#ifndef COMMON_H
#define COMMON_H

#include "stm32f0xx_hal.h"
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "matrixstate.h"
#include "fonts.h"
#include "buttons.h"
#include "SEGGER_RTT.h"
#include "semphr.h"
#include "flash.h"

#include <string.h>

extern FlashData_t flash_data;

/**
 * @brief Prints the contents of the FlashData_t structure to the RTT console.
 * 
 * @param data Pointer to the FlashData_t structure containing the data to print.
 * 
 * @note This function introduces small delays between prints to ensure RTT buffer stability.
 * which is done through vTaskDelay calls so it should be called from a task context.
 */
static inline void Flash_PrintData(const FlashData_t *data){
    SEGGER_RTT_WriteString(0, "=====================\n");
    SEGGER_RTT_WriteString(0, "Flash Data Contents:\n");
    SEGGER_RTT_WriteString(0, "=====================\n");
    SEGGER_RTT_printf(0, "Magic: 0x%08lX\n", data->magic);
    vTaskDelay(pdMS_TO_TICKS(10));
    SEGGER_RTT_printf(0, "Boot Count: %u\n", data->boot_count);
    vTaskDelay(pdMS_TO_TICKS(10));
    SEGGER_RTT_printf(0, "Version: %u\n", data->version);
    vTaskDelay(pdMS_TO_TICKS(10));
    SEGGER_RTT_printf(0, "Status: %u\n", data->status);
    vTaskDelay(pdMS_TO_TICKS(10));
    SEGGER_RTT_printf(0, "Counter: %lu\n", data->counter);
    vTaskDelay(pdMS_TO_TICKS(10));
    SEGGER_RTT_printf(0, "Snake High Score: %u\n", data->snake_high_score);
    vTaskDelay(pdMS_TO_TICKS(10));
    SEGGER_RTT_printf(0, "Bitris High Score: %u\n", data->bitris_high_score);
    vTaskDelay(pdMS_TO_TICKS(10));
    SEGGER_RTT_WriteString(0, "=====================\n");
    SEGGER_RTT_printf(0, "Checksum: 0x%08lX\n", data->checksum);
    SEGGER_RTT_WriteString(0, "=====================\n");
}

#endif // COMMON_H
