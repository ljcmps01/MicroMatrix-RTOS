#include "buttons.h"
#include "common.h"

Button sw2,sw3;

uint8_t counter=0;
uint8_t word[2][8]={0};

int get_char(char c){
    if(c>='A' && c<='Z'){
        return c-'A';
    }
    else if(c>='a' && c<='z'){
        return c-'A'-6;
    }
    else if(c>='0' && c<='9'){
        return c + 4;
    }
    else{
        switch(c){
            case ' ': return 62;
            case '!': return 63;
            case '?': return 64;
            case '/': return 65;
            case '.': return 66;
            case ':': return 67;
            case ';': return 68;
            default: return -1; //caracter no soportado
        }
    }
}

void load_char_to_screen(char* c){
    Matrix_t *m = GetMatrix();
    size_t a=strlen(c);
    for(int i=0; i<a; i++){
        counter=get_char(c[i]);
        SEGGER_RTT_printf(0, "Loading char: %c  (index %d)\n", c[i],counter);
        if(counter!=-1){
            load_output(m,letters[counter]);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}

void scroll_text(char* text){
    int location=0;
    Matrix_t *m = GetMatrix();
    size_t len=strlen(text);
    for(int i=0; i<len; i++){
        location=get_char(text[i]);
        if(location!=-1){
            for(int j=0; j<8; j++){
                word[1][j] = letters[location][j];
            }
            for(int j=0; j<8; j++){
                load_output(m,word[0]);
                for(int k=0; k<8; k++){
                    word[0][k] = (word[0][k] << 1);
                    if((1<<7) & word[1][k]){
                        word[0][k]++;
                    }
                    word[1][k] = (word[1][k] << 1);
                }
                shift_matrix(m,0);
                vTaskDelay(pdMS_TO_TICKS(100));
            }
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
    scroll_text("Hola! ::0123456789:: /Mundo desde Alpha?; App.");
    
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
