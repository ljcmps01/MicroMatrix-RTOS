#include <stdint.h>
#include <string.h>

#include "stm32f0xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "SEGGER_RTT.h"

#include "common.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

TaskHandle_t blinkHandle = NULL;
uint8_t led_state=0;

Button sw2,sw3;

uint8_t counter=0;


void ButtonHandler(const Button *btn, ButtonEvent_t event)
{
    Matrix_t *matrix = GetMatrix();
    if (btn == &sw2) {
        switch(event) {
            case BUTTON_EVENT_SHORT: 
                SEGGER_RTT_WriteString(0, "SW2 Short Press\n"); 
                if(counter<9) counter++;
                else counter=0;
                break;
            case BUTTON_EVENT_LONG:  
                SEGGER_RTT_WriteString(0, "SW2 Long Press\n"); 
                counter=9;
                break;
            case BUTTON_EVENT_DOUBLE:
                SEGGER_RTT_WriteString(0, "SW2 Double Tap\n"); 
                if(counter>0) counter--;
                else counter=9;
                break;
            default: break;
        }
    } else if (btn == &sw3) {
        switch(event) {
            case BUTTON_EVENT_SHORT: 
                SEGGER_RTT_WriteString(0, "SW3 Short Press\n");             
                if(counter>0) counter--;
                else counter=9;
                break;
            case BUTTON_EVENT_LONG:  
                SEGGER_RTT_WriteString(0, "SW3 Long Press\n"); 
                counter=0;
                break;
            case BUTTON_EVENT_DOUBLE:
                SEGGER_RTT_WriteString(0, "SW3 Double Tap\n"); 
                if(counter<9) counter++;
                else counter=0;
                break;
            default: break;
        }
    }
    load_output(matrix,digits[counter]);
}

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

/* RTT command handler task */
void vRTTTask(void *pvParameters)
{
    char buffer[32];
    
    SEGGER_RTT_WriteString(0, "RTT control task started.\n");
    SEGGER_RTT_WriteString(0, "Commands: on | off | toggle | manual | blink\n");

    for (;;)
    {
        // Non-blocking read from RTT channel 0
        int r = SEGGER_RTT_Read(0, buffer, sizeof(buffer)-1);
        if (r > 0) {
            buffer[r] = '\0'; // null terminate

            if (strncmp(buffer, "on", 2) == 0) {
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
                SEGGER_RTT_WriteString(0, "LED ON\n");
            }
            else if (strncmp(buffer, "off", 3) == 0) {
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
                SEGGER_RTT_WriteString(0, "LED OFF\n");
            }
            else if (strncmp(buffer, "toggle", 6) == 0) {
                HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);
                SEGGER_RTT_WriteString(0, "LED TOGGLED\n");
            }
            else if (strncmp(buffer, "manual", 6) == 0) {
                if (blinkHandle != NULL) {
                    vTaskSuspend(blinkHandle);
                    SEGGER_RTT_WriteString(0, "Blink task suspended. Manual control.\n");
                }
            }
            else if (strncmp(buffer, "blink", 5) == 0) {
                if (blinkHandle != NULL) {
                    vTaskResume(blinkHandle);
                    SEGGER_RTT_WriteString(0, "Blink task resumed.\n");
                }
            }
            else {
                SEGGER_RTT_WriteString(0, "Unknown command.\n");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // Poll RTT every 100 ms
    }
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    Matrix_Init(GetMatrix(),8,8,FILAS_GPIO_Port,COLUMNAS_GPIO_Port,FILAS_Pin,COLUMNAS_Pin,0);
    Button_Init(&sw2, BUTTON_GPIO_Port, SW2_Pin, ButtonHandler);
    Button_Init(&sw3, BUTTON_GPIO_Port, SW3_Pin, ButtonHandler);

    SEGGER_RTT_Init();  // <--- Initialize RTT buffer
    SEGGER_RTT_WriteString(0, "GPIO initialized. Type commands to interact!\n");

    MX_GPIO_Init();
    /* Create blink task */    
    SEGGER_RTT_printf(0, "Free heap before tasks: %u\n", (unsigned)xPortGetFreeHeapSize());

    xTaskCreate(vBlinkTask, "Blink", 128, NULL, 1, &blinkHandle);
    xTaskCreate(vRTTTask, "RTT", 256, NULL, 2, NULL);
    SEGGER_RTT_printf(0, "Free heap after tasks: %u\n", (unsigned)xPortGetFreeHeapSize());

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

