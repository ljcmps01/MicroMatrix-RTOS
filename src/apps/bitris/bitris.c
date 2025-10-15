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


typedef struct {
    uint8_t pos;
    Direction_t direction;
    uint8_t level;
    uint8_t gamescreen[8];
    BitrisState_t state;
    uint8_t max_level;
} BitrisScreen_t;

BitrisScreen_t BitrisInit(){
    BitrisScreen_t bitris;
    bitris.level = 1;
    bitris.max_level=8;
    for(size_t i=0;i<8;++i){
        bitris.gamescreen[i] = 0x00;
    }
    bitris.state = BITRIS_IDLE;
    bitris.direction = RIGHT;
    return bitris;
}

Button sw2,sw3;
BitrisScreen_t bitris;

void ButtonHandler(const Button *btn, ButtonEvent_t event){
    switch(event) {
        case BUTTON_EVENT_SHORT: 
            bitris.state = BITRIS_FALLING;
            break;
        default: break;
    }
}

void vBitrisTask(void *pvParameters){
    Matrix_t *matrix = GetMatrix();
    size_t pos = 0;
    for(;;)
    {
        switch(bitris.state){
            case BITRIS_IDLE:           // Player moving left and right
                
                bitris.gamescreen[0] = (1<<pos);
                bitris.direction?pos++:pos--;
                bitris.direction = (pos == 7)?LEFT:(pos == 0)?RIGHT:bitris.direction;
                
                load_output(matrix,bitris.gamescreen);
                vTaskDelay(pdMS_TO_TICKS(SPEED));
                break;

            case BITRIS_FALLING:        // Player falling    
                for(size_t i=0;i<(bitris.max_level-bitris.level);++i){
                    bitris.gamescreen[i] = (1<<pos);
                    load_output(matrix,bitris.gamescreen);
                    vTaskDelay(pdMS_TO_TICKS(SPEED/2));
                }
                for(size_t i=1;i<(bitris.max_level-bitris.level);++i){
                    bitris.gamescreen[i] = 0x00;
                }
                bitris.state=BITRIS_LANDED;    
                break;

            case BITRIS_LANDED:         // Player landed
                bitris.gamescreen[bitris.max_level-bitris.level]|=bitris.gamescreen[0];
                load_output(matrix,bitris.gamescreen);
                bitris.state=BITRIS_CLEARING;    
                break;

            case BITRIS_CLEARING:       // Clearing lines
                if(bitris.gamescreen[bitris.max_level-bitris.level]==255)
                    bitris.level++;
                bitris.state=BITRIS_IDLE;    
                break;
            case BITRIS_GAMEOVER:       // Game over
            default:
                break;
        }
    }
}

void RunApp(void)
{
    bitris = BitrisInit();
    xTaskCreate(vBitrisTask, "Bitris", 256, NULL, 2, NULL);
    Button_Init(&sw2, BUTTON_GPIO_Port, SW2_Pin, ButtonHandler);
    Button_Init(&sw3, BUTTON_GPIO_Port, SW3_Pin, ButtonHandler);
}