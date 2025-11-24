#include "bitris.h"
#include "common.h"

#define SPEED 100
#define MAX_LEVEL 8

//soft limits for score calculation
#define MAX_CLICKS 100
#define MAX_DURATION 300000

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
    BITRIS_GAMEOVER,
    BITRIS_STALL,
    BITRIS_FLASH_UPDATE
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

    uint8_t score;

    uint16_t player_precision;
}Stadistics_t;

void StadisticsInit (Stadistics_t *stats){
    stats->game_duration_total = pdTICKS_TO_MS(xTaskGetTickCount());
    stats->game_clicks_total = 0;

    stats->game_duration_per_level[0] = 0;

    for (uint8_t i = 0; i < MAX_LEVEL; i++)
    {
        stats->game_clicks_per_level[i] = 0;
    }

    stats->failed_clicks = 0;
    
    stats->player_precision = 0;
}

void StadisticsPrint (Stadistics_t *stats){
    SEGGER_RTT_WriteString(0,"=================================\n");
    SEGGER_RTT_WriteString(0,"\tGAME STATS\n");
    SEGGER_RTT_WriteString(0,"=================================\n");
    SEGGER_RTT_printf(0,"Game duration:\t\t%ds\n", stats->game_duration_total/1000);
    SEGGER_RTT_printf(0,"Total clicks made:\t%d\n", stats->game_clicks_total);
    SEGGER_RTT_printf(0,"Failed clicks:\t\t%d\n", stats->failed_clicks);
    SEGGER_RTT_printf(0,"Precision rate:\t\t%d%%\n\n", stats->failed_clicks*100/stats->game_clicks_total);

    vTaskDelay(pdMS_TO_TICKS(25));
    SEGGER_RTT_printf(0,"Final Score:\t\t%d\n", stats->score);
    SEGGER_RTT_printf(0,"High Score:\t\t%d\n", flash_data.bitris_high_score);

    SEGGER_RTT_WriteString(0,"=================================\n");
    SEGGER_RTT_WriteString(0,"\tPER LEVEL STATS\n");
    SEGGER_RTT_WriteString(0,"=================================\n");
    vTaskDelay(pdMS_TO_TICKS(25));
    for (uint8_t i = 0; i < MAX_LEVEL-1; i++)
    {
        SEGGER_RTT_printf(0,"\t\tLEVEL  %d\n",i);
        SEGGER_RTT_WriteString(0,"=================================\n");
        SEGGER_RTT_printf(0,"Game duration:\t%ds\n", stats->game_duration_per_level[i]/1000);
        SEGGER_RTT_printf(0,"Game clicks:\t%d\n", stats->game_clicks_per_level[i]);
        SEGGER_RTT_WriteString(0,"=================================\n");
        vTaskDelay(pdMS_TO_TICKS(25));
    }
    
}

/**
 * @brief Calculates the score based on game statistics.
 * 
 * @param stats Pointer to the Stadistics_t structure containing game statistics.
 * @return uint8_t Returns 1 if a new high score is achieved, otherwise 0.
 */
uint8_t ScoreCalculation(Stadistics_t *stats){
    //Precision score
    float precision_score = 1.0f - (float)stats->failed_clicks/(float)MAX_CLICKS;
    //Duration score (convert milliseconds to seconds)
    float duration_score = 1.0f - (float)(stats->game_duration_total)/(float)MAX_DURATION;

    if (precision_score < 0) precision_score = 0;
    if (duration_score < 0) duration_score = 0;

    float raw_score = precision_score * 0.7f + duration_score * 0.3f;
    
    stats->score = (uint8_t)(raw_score * 64);

    if (stats->score > flash_data.bitris_high_score)
    {
        stats->high_score = stats->score;
        return 1;
    }
    
    return 0;
}

void BitrisInit(BitrisScreen_t *bitris){
    bitris->pos=0;
    bitris->level = 1;
    bitris->max_level=MAX_LEVEL;
    for(size_t i=0;i<MAX_LEVEL;++i){
        bitris->gamescreen[i] = 0x00;
    }
    bitris->state = BITRIS_IDLE;
    bitris->direction = RIGHT;
}

Button sw2,sw3;
BitrisScreen_t bitris;
Stadistics_t stadistics;

void ButtonHandler(const Button *btn, ButtonEvent_t event){
    switch(event) {
        case BUTTON_EVENT_PRESS:
            if (bitris.state == BITRIS_STALL) {
                StadisticsInit(&stadistics);
                BitrisInit(&bitris);
                bitris.state = BITRIS_IDLE;
            }
            else {
                bitris.state = BITRIS_FALLING;
            }
            break;
        default:
            break;
    }
}

void BitrisScore(uint16_t score){
    uint8_t full_rows = score / 8;
    uint8_t remainder = score % 8;

    for (size_t i = 0; i < full_rows; i++)
    {
        bitris.gamescreen[i] = 0xFF;
    }
    if (full_rows < 8)
    {
        bitris.gamescreen[full_rows] = (1 << remainder) - 1;
    }
    
    
}

void vBitrisTask(void *pvParameters){
    Matrix_t *matrix = GetMatrix();
    Stadistics_t stadistics = StadisticsInit();
    
    /* Print flash data after a short delay to ensure scheduler is fully running */
    vTaskDelay(pdMS_TO_TICKS(100));
    Flash_PrintData(&flash_data);
    
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
                        (pdTICKS_TO_MS(xTaskGetTickCount()) - stadistics.game_duration_total):
                        (pdTICKS_TO_MS(xTaskGetTickCount()) - stadistics.game_duration_per_level[bitris.level-2]);
                    bitris.level++;
                }
                bitris.state=bitris.level==MAX_LEVEL?BITRIS_GAMEOVER:BITRIS_IDLE;    
                break;
            case BITRIS_GAMEOVER:       // Game over
                //Closes stats
                stadistics.game_duration_total = (pdTICKS_TO_MS(xTaskGetTickCount()) - stadistics.game_duration_total);

                uint8_t new_hs = ScoreCalculation(&stadistics);
                StadisticsPrint(&stadistics);
                BitrisScore(stadistics.score);
                
                if (new_hs)
                {
                    bitris.state = BITRIS_FLASH_UPDATE;
                    SEGGER_RTT_printf(0,"New High Score Achieved! %d > %d\n", stadistics.high_score, flash_data.bitris_high_score);
                    vTaskDelay(pdMS_TO_TICKS(25));
                }
                else
                {
                    BitrisInit(&bitris);
                    stadistics = StadisticsInit();
                    bitris.state=BITRIS_STALL;
                }
                break;
            case BITRIS_FLASH_UPDATE:
                flash_data.bitris_high_score = stadistics.high_score;
                if(Flash_Save(&flash_data) == HAL_OK){
                    vTaskDelay(pdMS_TO_TICKS(25));
                    SEGGER_RTT_WriteString(0,"High Score saved!\n");
                }
                else{
                    vTaskDelay(pdMS_TO_TICKS(25));
                    SEGGER_RTT_WriteString(0,"Failed to save High Score!\n");
                }

                BitrisInit(&bitris);
                stadistics = StadisticsInit();
                bitris.state=BITRIS_STALL;
                break;
            case BITRIS_STALL:
                load_output(matrix,bitris.gamescreen);
                
                vTaskDelay(pdMS_TO_TICKS(100));
                break;
            default:
                break;
        }
    }
}

void RunApp(void)
{
    BitrisInit(&bitris);
    xTaskCreate(vBitrisTask, "Bitris", 256, NULL, 2, NULL);
    Button_Init(&sw2, BUTTON_GPIO_Port, SW2_Pin, ButtonHandler);
    Button_Init(&sw3, BUTTON_GPIO_Port, SW3_Pin, ButtonHandler);
}