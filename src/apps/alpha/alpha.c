#include "buttons.h"
#include "common.h"

Button sw2,sw3;
uint8_t counter=0;

void ButtonHandler(const Button *btn, ButtonEvent_t event)
{
    Matrix_t *matrix = GetMatrix();
    if (btn == &sw2) {
        switch(event) {
            case BUTTON_EVENT_SHORT: 
                SEGGER_RTT_WriteString(0, "SW2 Short Press\n"); 
                if(counter<52) counter++;
                else counter=0;
                break;
            case BUTTON_EVENT_LONG:  
                SEGGER_RTT_WriteString(0, "SW2 Long Press\n"); 
                counter=52;
                break;
            case BUTTON_EVENT_DOUBLE:
                SEGGER_RTT_WriteString(0, "SW2 Double Tap\n"); 
                if(counter>0) counter--;
                else counter=52;
                break;
            default: break;
        }
    } else if (btn == &sw3) {
        switch(event) {
            case BUTTON_EVENT_SHORT: 
                SEGGER_RTT_WriteString(0, "SW3 Short Press\n");             
                if(counter>0) counter--;
                else counter=52;
                break;
            case BUTTON_EVENT_LONG:  
                SEGGER_RTT_WriteString(0, "SW3 Long Press\n"); 
                counter=0;
                break;
            case BUTTON_EVENT_DOUBLE:
                SEGGER_RTT_WriteString(0, "SW3 Double Tap\n"); 
                if(counter<52) counter++;
                else counter=0;
                break;
            default: break;
        }
    }
    load_output(matrix,letters[counter]);
}

void vAlphaTask(void *pvParameters)
{
    scroll_text("Hola! ::0123456789:: /Mundo desde Alpha?; App.");
}

void RunApp(void)
{
    Button_Init(&sw2, BUTTON_GPIO_Port, SW2_Pin, ButtonHandler);
    Button_Init(&sw3, BUTTON_GPIO_Port, SW3_Pin, ButtonHandler);

    xTaskCreate(vAlphaTask, "AlphaTask", 128, NULL, tskIDLE_PRIORITY + 1, NULL);
}
