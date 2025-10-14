#include "bitris.h"
#include "common.h"

#define SPEED 100

typedef enum {
    LEFT,
    RIGHT
}Direction_t;

typedef enum {
    BITRIS_IDLE,
    BITRIS_FALLING,
    BITRIS_LANDED,
    BITRIS_CLEARING,
    BITRIS_GAMEOVER
}BitrisState_t;

Button sw2,sw3;
uint8_t gamescreen[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
BitrisState_t state = BITRIS_IDLE;

void ButtonHandler(const Button *btn, ButtonEvent_t event){
    switch(event) {
        case BUTTON_EVENT_SHORT: 
            state = BITRIS_FALLING;
            break;
        default: break;
    }
}

void vBitrisTask(void *pvParameters){
    Matrix_t *matrix = GetMatrix();
    Direction_t direction = RIGHT;
    uint8_t *player = &gamescreen[0];
    size_t pos = 0;
    for(;;)
    {
        switch(state){
            case BITRIS_IDLE:           // Player moving left and right
                Matrix_Clear(matrix);
                *player = (1<<pos);
                direction?pos++:pos--;
                direction = (pos == 7)?LEFT:(pos == 0)?RIGHT:direction;
                
                load_output(matrix,gamescreen);
                vTaskDelay(pdMS_TO_TICKS(SPEED));
                break;
            case BITRIS_FALLING:        // Player falling    
                for(size_t i=0;i<8;++i){
                    gamescreen[i] = *player;
                    load_output(matrix,gamescreen);
                    vTaskDelay(pdMS_TO_TICKS(SPEED/2));
                }
                state=BITRIS_IDLE;    
                break;
            case BITRIS_LANDED:         // Player landed
            case BITRIS_CLEARING:       // Clearing lines
            case BITRIS_GAMEOVER:       // Game over
            default:
                break;
        }
    }
}

void RunApp(void)
{
    xTaskCreate(vBitrisTask, "Bitris", 256, NULL, 2, NULL);
    Button_Init(&sw2, BUTTON_GPIO_Port, SW2_Pin, ButtonHandler);
    Button_Init(&sw3, BUTTON_GPIO_Port, SW3_Pin, ButtonHandler);
}