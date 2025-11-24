#include "buttons.h"
#include "common.h"

Button sw2,sw3;

void ButtonHandler(const Button *btn, ButtonEvent_t event)
{
    Matrix_t *matrix = GetMatrix();
    uint32_t last_counter = flash_data.counter;
    if (btn == &sw2) {
        switch(event) {
            case BUTTON_EVENT_SHORT: 
                SEGGER_RTT_WriteString(0, "SW2 Short Press\n"); 
                if(flash_data.counter<9) flash_data.counter++;
                else flash_data.counter=0;
                break;
            case BUTTON_EVENT_LONG:  
                SEGGER_RTT_WriteString(0, "SW2 Long Press\n"); 
                flash_data.counter=9;
                break;
            case BUTTON_EVENT_DOUBLE:
                SEGGER_RTT_WriteString(0, "SW2 Double Tap\n"); 
                if(flash_data.counter>0) flash_data.counter--;
                else flash_data.counter=9;
                break;
            default: break;
        }
    } else if (btn == &sw3) {
        switch(event) {
            case BUTTON_EVENT_SHORT: 
                SEGGER_RTT_WriteString(0, "SW3 Short Press\n");             
                if(flash_data.counter>0) flash_data.counter--;
                else flash_data.counter=9;
                break;
            case BUTTON_EVENT_LONG:  
                SEGGER_RTT_WriteString(0, "SW3 Long Press\n"); 
                flash_data.counter=0;
                break;
            case BUTTON_EVENT_DOUBLE:
                SEGGER_RTT_WriteString(0, "SW3 Double Tap\n"); 
                if(flash_data.counter<9) flash_data.counter++;
                else flash_data.counter=0;
                break;
            default: break;
        }
    }
    if(last_counter != flash_data.counter){
        SEGGER_RTT_printf(0, "Counter: %lu\n", flash_data.counter);
        Flash_Save(&flash_data);
        load_output(matrix,letters[flash_data.counter%10+52]); //Calculate the leftover over 10 in case counter overflows
    }
}

void RunApp(void)
{
    Button_Init(&sw2, BUTTON_GPIO_Port, SW2_Pin, ButtonHandler);
    Button_Init(&sw3, BUTTON_GPIO_Port, SW3_Pin, ButtonHandler);
}