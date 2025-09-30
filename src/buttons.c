#include "buttons.h"
#include "common.h"

void vButtonTask(void *pvParameters)
{
    Matrix_t *matrix = (Matrix_t *)pvParameters;
    static uint8_t prev_sw2 = 0, prev_sw3 = 0, counter = 0;

    for(;;)
    {
        uint8_t sw2_state = HAL_GPIO_ReadPin(BUTTON_GPIO_Port, SW2_Pin);
        uint8_t sw3_state = HAL_GPIO_ReadPin(BUTTON_GPIO_Port, SW3_Pin);

        if (sw2_state != prev_sw2) {
            SEGGER_RTT_printf(0, "SW2 toggled: now %d\n", sw2_state);
            prev_sw2 = sw2_state;
            if (sw2_state == 0 && counter<9) { // Button pressed
                counter++;
                SEGGER_RTT_printf(0, "Counter incremented: %d\n", counter);
                load_output(matrix,digits[counter]);
            }
        }

        if (sw3_state != prev_sw3) {
            SEGGER_RTT_printf(0, "SW3 toggled: now %d\n", sw3_state);
            prev_sw3 = sw3_state;
            if (sw3_state == 0 && counter>0) { // Button pressed
                counter--;
                SEGGER_RTT_printf(0, "Counter decreased: %d\n", counter);
                load_output(matrix,digits[counter]);
            }
        }

        load_output(matrix,digits[counter]);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}