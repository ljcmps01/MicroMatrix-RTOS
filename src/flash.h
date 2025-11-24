#ifndef __FLASH_H
#define __FLASH_H

#include "stm32f0xx_hal.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* Flash memory configuration for STM32F051 */
#define FLASH_USER_START_ADDR   0x0800F800  /* Start of user data area (Page 31, last 2KB) */
#define FLASH_USER_END_ADDR     0x0800FFFF  /* End of flash */

/* Task priorities */
#define FLASH_WRITE_TASK_PRIORITY   (tskIDLE_PRIORITY + 2)
#define FLASH_READ_TASK_PRIORITY    (tskIDLE_PRIORITY + 1)

/* Flash data magic number for validation */
#define FLASH_DATA_MAGIC    0xDEADBEEF
#define FLASH_DATA_VERSION  1

typedef enum {
    FLASH_STATUS_OK = 0,
    FLASH_STATUS_ERROR,
    FLASH_STATUS_INVALID,
} FlashStatus_t;

/* Generic persistent data structure to store in flash */
typedef struct {
    uint32_t magic;              /* Magic number to validate data integrity */
    uint16_t boot_count;         /* Number of times device has booted */
    uint8_t version;             /* Data structure version for future compatibility */
    uint8_t status;              /* General status byte */
    uint32_t counter;
    
    /* App-specific data */
    uint8_t bitris_high_score;         /* High score for games */
    uint8_t snake_high_score;         /* High score for games */
    uint16_t reserved;           /* Reserved for future use */

    
    uint32_t checksum;           /* Simple checksum for data validation */
} FlashData_t;

/* Mutex for flash access protection */
extern SemaphoreHandle_t xFlashMutex;

void FlashData_Init(FlashData_t *data);
uint8_t Flash_Save(FlashData_t *data);
FlashStatus_t Flash_Load(FlashData_t *data);

#endif // __FLASH_H