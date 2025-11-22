#include "flash.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include <string.h>

/* Global mutex definition */
SemaphoreHandle_t xFlashMutex = NULL;

/****************** Helper Functions for FlashData_t ******************/

/* Initialize flash data structure with default values */
void FlashData_Init(FlashData_t *data)
{
    memset(data, 0, sizeof(FlashData_t));
    data->magic = FLASH_DATA_MAGIC;
    data->version = FLASH_DATA_VERSION;
    for (size_t i = 0; i < 2; i++)
    {
        data->high_score[i] = 0;
    }
    
    data->boot_count = 0;
    data->checksum = FlashData_CalculateChecksum(data);
}

/* Calculate checksum for flash data (excluding checksum field itself) */
uint32_t FlashData_CalculateChecksum(FlashData_t *data)
{
    uint32_t checksum = 0;
    uint8_t *ptr = (uint8_t *)data;
    size_t size = sizeof(FlashData_t) - sizeof(data->checksum);
    
    for (size_t i = 0; i < size; i++)
    {
        checksum += ptr[i];
    }
    
    return checksum;
}

/* Validate flash data integrity */
uint8_t FlashData_Validate(FlashData_t *data)
{
    /* Check magic number */
    if (data->magic != FLASH_DATA_MAGIC)
    {
        return 0;  /* Invalid magic */
    }
    
    /* Check version compatibility */
    if (data->version > FLASH_DATA_VERSION)
    {
        return 0;  /* Unsupported version */
    }
    
    /* Verify checksum */
    uint32_t calculated_checksum = FlashData_CalculateChecksum(data);
    if (calculated_checksum != data->checksum)
    {
        return 0;  /* Checksum mismatch */
    }
    
    return 1;  /* Valid data */
}

/********************* METHOD 1: STM32 HAL Functions *********************/

/* Erase flash page using HAL */
HAL_StatusTypeDef Flash_Erase_HAL(uint32_t PageAddress)
{
    HAL_StatusTypeDef status;
    FLASH_EraseInitTypeDef eraseInitStruct;
    uint32_t pageError;
    
    HAL_FLASH_Unlock();
    
    eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInitStruct.PageAddress = PageAddress;
    eraseInitStruct.NbPages = 1;
    
    status = HAL_FLASHEx_Erase(&eraseInitStruct, &pageError);
    
    HAL_FLASH_Lock();
    
    return status;
}

/* Write data to flash using HAL */
HAL_StatusTypeDef Flash_Write_HAL(uint32_t StartAddress, uint32_t *Data, uint16_t NumberOfWords)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint16_t i;
    
    HAL_FLASH_Unlock();
    
    for (i = 0; i < NumberOfWords; i++)
    {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 
                                   StartAddress + (i * 4), 
                                   Data[i]);
        
        if (status != HAL_OK)
        {
            break;
        }
    }
    
    HAL_FLASH_Lock();
    
    return status;
}


/****************** Common Read Function ******************/

/* Read data from flash memory (no HAL needed - direct memory access) */
void Flash_Read_Data(uint32_t StartAddress, uint32_t *Data, uint16_t NumberOfWords)
{
    uint16_t i;
    
    for (i = 0; i < NumberOfWords; i++)
    {
        Data[i] = *(__IO uint32_t*)(StartAddress + (i * 4));
    }
}

uint8_t Flash_Save(FlashData_t *data){
    uint32_t writeBuffer[sizeof(FlashData_t) / sizeof(uint32_t) + 1];
    HAL_StatusTypeDef status;
    
    data->checksum = FlashData_CalculateChecksum(data);
            
    memcpy(writeBuffer, data, sizeof(FlashData_t));
            
    status = Flash_Erase_HAL(FLASH_USER_START_ADDR);
    if (status == HAL_OK)
    {
        status = Flash_Write_HAL(FLASH_USER_START_ADDR, 
                                writeBuffer, 
                                sizeof(FlashData_t) / sizeof(uint32_t) + 1);
        if (status == HAL_OK);
    }
    return status;
}

FlashStatus_t Flash_Load(FlashData_t *data){
    uint32_t readBuffer[sizeof(FlashData_t) / sizeof(uint32_t) + 1];
    
    Flash_Read_Data(FLASH_USER_START_ADDR, 
                    readBuffer, 
                    sizeof(FlashData_t) / sizeof(uint32_t) + 1);
    
    memcpy(data, readBuffer, sizeof(FlashData_t));
    
    if (FlashData_Validate(data))
    {
        return FLASH_STATUS_OK;
    }
    else
    {
        FlashData_Init(data);
        return FLASH_STATUS_INVALID;
    }
    return FLASH_STATUS_ERROR;
}
