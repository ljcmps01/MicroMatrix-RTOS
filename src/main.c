#include <stdint.h>

#include "common.h"
#if COUNTER
#include "counter.h"
#elif BITRIS
#include "bitris.h"
#elif ALPHA
#include "alpha.h"
#elif SNAKE
#include "snake.h"
#endif

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

TaskHandle_t blinkHandle = NULL;
uint8_t led_state=0;

FlashData_t flash_data;

/* Blink task */
void vBlinkTask(void *pvParameters)
{
    for(;;)
    {
        led_state=~led_state;
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);  // Example: LED on PC8
        SEGGER_RTT_WriteString(0, "LED toggled\n");
        vTaskDelay(pdMS_TO_TICKS(1000));         // 500 ms delay
    }
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    Matrix_Init(GetMatrix(),8,8,FILAS_GPIO_Port,COLUMNAS_GPIO_Port,FILAS_Pin,COLUMNAS_Pin,0);

    SEGGER_RTT_Init();  // <--- Initialize RTT buffer
    SEGGER_RTT_WriteString(0, "GPIO and RTT initialized.\n");

    MX_GPIO_Init();

    /* Create mutex for flash access */
    xFlashMutex = xSemaphoreCreateMutex();
    
    if (xFlashMutex != NULL)
    {
        SEGGER_RTT_WriteString(0, "\033[92m\033[1mFlash mutex created successfully.\033[0m\n");

        FlashStatus_t status = Flash_Load(&flash_data);

        if(status == FLASH_STATUS_INVALID){
            SEGGER_RTT_WriteString(0, "Flash data invalid. Initializing to defaults and saving...\n");
            /* Save the initialized data to flash so next boot will be valid */
            uint8_t save_status = Flash_Save(&flash_data);
            if (save_status == HAL_OK) {
                SEGGER_RTT_WriteString(0, "Default flash data saved successfully.\n");
            } else {
                SEGGER_RTT_WriteString(0, "Warning: Failed to save default flash data.\n");
            }
        }
        else
        {
            SEGGER_RTT_WriteString(0, "Flash data valid.\n");
        }
    }
    else
    {
        FlashData_Init(&flash_data);
        SEGGER_RTT_WriteString(0, "Failed to create flash mutex.\n");
        SEGGER_RTT_WriteString(0, "\033[93m\033[1mMicroMatrix will work but data will not be persistent.\033[0m\n");
    }

    #if COUNTER
    SEGGER_RTT_WriteString(0, "Start Counter App\n");
    RunApp(); // Start the counter app
    #elif BITRIS
    SEGGER_RTT_WriteString(0, "Start Bitris App\n");
    RunApp(); // Start the bitris app
    #elif ALPHA
    SEGGER_RTT_WriteString(0, "Start Alpha App\n");
    RunApp(); // Start the alpha app
    #elif SNAKE
    SEGGER_RTT_WriteString(0, "Start Snake App\n");
    RunApp(); // Start the snake app
    #else
    SEGGER_RTT_WriteString(0, "No app selected.\n");
    #endif
    /* Start scheduler */
    vTaskStartScheduler();

    /* Should never reach here */
    while (1) {}
}


void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    for(;;); // halt here
}

void vApplicationIdleHook(void)
{
    SEGGER_RTT_printf(0, "Heap free before tasks: %u\n", xPortGetFreeHeapSize());
    vTaskDelay(pdMS_TO_TICKS(1000)); // Print every second
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;    
    SEGGER_RTT_printf(0, "Stack overflow in task: %s\n", pcTaskName);
    taskDISABLE_INTERRUPTS();   
    for(;;); // halt here
}

/* System clock config (48 MHz HSI/PLL for STM32F051) */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12; // 8 MHz / 2 * 12 = 48 MHz
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
}

/* GPIO init (PC8 as output) */
static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, FILAS_Pin|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                            |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, COLUMNAS_Pin|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                            |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8, GPIO_PIN_RESET);

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


    /*Configure GPIO pins : SW2_Pin SW3_Pin */
    GPIO_InitStruct.Pin = SW2_Pin|SW3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(BUTTON_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : FILAS_Pin PA1 PA2 PA3
                            PA4 PA5 PA6 PA7 */
    GPIO_InitStruct.Pin = FILAS_Pin|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                            |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : COLUMNAS_Pin PB1 PB2 PB3
                            PB4 PB5 PB6 PB7 */
    GPIO_InitStruct.Pin = COLUMNAS_Pin|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                            |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    // SEGGER_RTT_printf(0, "Free heap: %u\n", xPortGetFreeHeapSize());
}

