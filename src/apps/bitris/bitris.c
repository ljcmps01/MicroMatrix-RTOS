#include "bitris.h"
#include "common.h"

typedef enum {
    LEFT,
    RIGHT
}Direction_t;

Button sw2,sw3;
uint8_t gamescreen[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

void vBitrisTask(void *pvParameters){
    Matrix_t *matrix = GetMatrix();
    uint8_t player;
    Direction_t direction=RIGHT;
    size_t pos=0;
    for(;;)
    {
        player=(1<<pos);
        gamescreen[0]=player;
        direction?pos++:pos--;
        direction=(pos==7)?LEFT:(pos==0)?RIGHT:direction;
        
        load_output(matrix,gamescreen);
        vTaskDelay(pdMS_TO_TICKS(200));         // 200 ms delay
    }
}

void RunApp(void)
{
    xTaskCreate(vBitrisTask, "Bitris", 256, NULL, 2, NULL);
}