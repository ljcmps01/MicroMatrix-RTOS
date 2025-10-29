#include "bitris.h"
#include "common.h"

#define SPEED 100
#define MAX_LEVEL 8

/*TODO:
- Implement lives
- Implement score   (Could be done through RTT until fonts is implemented)

- Add animations (score, fail, gameover, etc)
- Implement LedState for visual score indication
- Add pause state
    - For gameover pause
    - Button gesture to pause the game
*/

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
    uint8_t gamescreen[MAX_LEVEL];
    BitrisState_t state;
    uint8_t max_level;
} BitrisScreen_t;

typedef struct {
    uint32_t game_duration_total;
    uint32_t game_duration_per_level[MAX_LEVEL];

    uint16_t game_clicks_total;
    uint16_t game_clicks_per_level[MAX_LEVEL];
    uint16_t failed_clicks;

    uint16_t player_precision;
}Stadistics_t;

Stadistics_t StadisticsInit (){
    Stadistics_t new_stats;
    new_stats.game_duration_total = xTaskGetTickCount();
    new_stats.game_clicks_total = 0;

    new_stats.game_duration_per_level[0] = 0;

    for (uint8_t i = 0; i < MAX_LEVEL; i++)
    {
        new_stats.game_clicks_per_level[i] = 0;
    }

    new_stats.failed_clicks = 0;
    
    new_stats.player_precision = 0;

    return new_stats;
}

void StadisticsPrint (Stadistics_t stats){
    SEGGER_RTT_WriteString(0,"=================================\n");
    SEGGER_RTT_WriteString(0,"\tGAME STATS\n");
    SEGGER_RTT_WriteString(0,"=================================\n");
    SEGGER_RTT_printf(0,"Game duration:\t\t%ds\n", stats.game_duration_total);
    SEGGER_RTT_printf(0,"Total clicks made:\t%d\n", stats.game_clicks_total);
    SEGGER_RTT_printf(0,"Failed clicks:\t\t%d\n", stats.failed_clicks);
    SEGGER_RTT_printf(0,"Precision rate:\t\t%d\n\n", stats.failed_clicks*100/stats.game_clicks_total);


    SEGGER_RTT_WriteString(0,"=================================\n");
    SEGGER_RTT_WriteString(0,"\tPER LEVEL STATS\n");
    SEGGER_RTT_WriteString(0,"=================================\n");
    for (uint8_t i = 0; i < MAX_LEVEL-1; i++)
    {
        SEGGER_RTT_printf(0,"\t\tLEVEL  %d\n",i);
        SEGGER_RTT_WriteString(0,"=================================\n");
        SEGGER_RTT_printf(0,"Game duration:\t%d\n", stats.game_duration_per_level[i]);
        SEGGER_RTT_printf(0,"Game clicks:\t%d\n", stats.game_clicks_per_level[i]);
        SEGGER_RTT_WriteString(0,"=================================\n");
            
    }
    
}

BitrisScreen_t BitrisInit(){
    BitrisScreen_t new_bitris;
    new_bitris.pos=0;
    new_bitris.level = 1;
    new_bitris.max_level=MAX_LEVEL;
    for(size_t i=0;i<MAX_LEVEL;++i){
        new_bitris.gamescreen[i] = 0x00;
    }
    new_bitris.state = BITRIS_IDLE;
    new_bitris.direction = RIGHT;
    return new_bitris;
}

Button sw2,sw3;
BitrisScreen_t bitris;

void ButtonHandler(const Button *btn, ButtonEvent_t event){
    switch(event) {
        case BUTTON_EVENT_PRESS:
            bitris.state = BITRIS_FALLING;
        default:
            break;
    }
}

void vBitrisTask(void *pvParameters){
    Matrix_t *matrix = GetMatrix();
    Stadistics_t stadistics = StadisticsInit();
    for(;;)
    {
        switch(bitris.state){
            case BITRIS_IDLE:           // Player moving left and right
                
                bitris.gamescreen[0] = (1<<bitris.pos);
                bitris.direction?bitris.pos++:bitris.pos--;
                bitris.direction = (bitris.pos == 7)?LEFT:(bitris.pos == 0)?RIGHT:bitris.direction;
                
                load_output(matrix,bitris.gamescreen);
                vTaskDelay(pdMS_TO_TICKS(SPEED-(bitris.level*10)));
                break;

            case BITRIS_FALLING:        // Player falling    
                for(size_t i=0;i<(bitris.max_level-bitris.level);++i){
                    bitris.gamescreen[i] = (1<<bitris.pos);
                    load_output(matrix,bitris.gamescreen);
                    vTaskDelay(pdMS_TO_TICKS(SPEED/2));
                }
                for(size_t i=1;i<(bitris.max_level-bitris.level);++i){
                    bitris.gamescreen[i] = 0x00;
                }
                bitris.state=BITRIS_LANDED;    
                stadistics.game_clicks_total++;
                stadistics.game_clicks_per_level[bitris.level-1]++;
                break;

            case BITRIS_LANDED:         // Player landed
                uint8_t hit = bitris.gamescreen[bitris.max_level-bitris.level];
                bitris.gamescreen[bitris.max_level-bitris.level]|=bitris.gamescreen[0];

                if(hit == bitris.gamescreen[bitris.max_level-bitris.level])
                    stadistics.failed_clicks++;

                load_output(matrix,bitris.gamescreen);
                bitris.state=BITRIS_CLEARING;    
                break;

            case BITRIS_CLEARING:       // Clearing lines
                if(bitris.gamescreen[bitris.max_level-bitris.level]==255){
                    stadistics.game_duration_per_level[bitris.level-1] = bitris.level == 0?
                        (xTaskGetTickCount() - stadistics.game_duration_total) / configTICK_RATE_HZ:
                        (xTaskGetTickCount() - stadistics.game_duration_per_level[bitris.level-2]) / configTICK_RATE_HZ;
                    bitris.level++;
                }
                bitris.state=bitris.level==MAX_LEVEL?BITRIS_GAMEOVER:BITRIS_IDLE;    
                break;
            case BITRIS_GAMEOVER:       // Game over
                //Closes stats
                stadistics.game_duration_total = (xTaskGetTickCount() - stadistics.game_duration_total) / configTICK_RATE_HZ;
                StadisticsPrint(stadistics);
                stadistics = StadisticsInit();
                bitris = BitrisInit();

                bitris.state=BITRIS_IDLE;
                break;
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