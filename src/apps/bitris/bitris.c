#include "bitris.h"
#include "common.h"

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

void vBitrisTask(void *pvParameters){
    Matrix_t *matrix = GetMatrix();
    Direction_t direction=RIGHT;
    uint8_t *player=&gamescreen[0];
    size_t pos=0;
    BitrisState_t state=BITRIS_IDLE;
    for(;;)
    {
        switch(state){
            case BITRIS_IDLE:           // Player moving left and right
                *player=(1<<pos);
                direction?pos++:pos--;
                direction=(pos==7)?LEFT:(pos==0)?RIGHT:direction;
                
                load_output(matrix,gamescreen);
                vTaskDelay(pdMS_TO_TICKS(200));         // 200 ms delay
                break;
            case BITRIS_FALLING:        // Player falling
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
}