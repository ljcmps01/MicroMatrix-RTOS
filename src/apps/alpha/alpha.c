#include "buttons.h"
#include "common.h"

Button sw2,sw3;

uint8_t counter=0;

int get_char(char c){
    if(c>='A' && c<='Z'){
        return c-'A';
    }
    else if(c>='a' && c<='z'){
        return c-'A'-6;
    }
    else{
        return -1; //caracter no soportado
    }
}

void load_char_to_screen(char* c){
    Matrix_t *m = GetMatrix();
    int a=3;
    for(int i=0; i<a; i++){
        load_output(m,letters[counter]);
        SEGGER_RTT_printf(0, "Loading char: %c\n", c[i]);
        vTaskDelay(pdMS_TO_TICKS(500));
        counter=get_char(c[i]);
        SEGGER_RTT_printf(0, "Loading char: %d\n", counter);
        if(counter!=-1){
            load_output(m,letters[counter]);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}

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
    load_char_to_screen("HolA MuNdO");
    
    // After displaying, you can loop or delete the task
    for(;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void RunApp(void)
{
    Button_Init(&sw2, BUTTON_GPIO_Port, SW2_Pin, ButtonHandler);
    Button_Init(&sw3, BUTTON_GPIO_Port, SW3_Pin, ButtonHandler);

    xTaskCreate(vAlphaTask, "AlphaTask", 128, NULL, tskIDLE_PRIORITY + 1, NULL);
}
