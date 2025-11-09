#include "snake.h"
#include "common.h"

#define SPEED 250
#define MAX_LEVEL 8
#define MAX_SNAKE_LEN 32

#define SNAKE_INITIAL_LEN 1

typedef enum {
    HORIZONTAL,
    VERTICAL
}Direction_t;

typedef enum{
    NEGATIVE,
    POSITIVE
}Sense_t;

typedef enum {
    SNAKE_IDLE,
    SNAKE_MOVING,
    SNAKE_GROWING,
    SNAKE_GAMEOVER,
    SNAKE_STALL
}SnakeState_t;

typedef struct{
    uint8_t length;
    uint8_t head_x;
    uint8_t head_y;
    Direction_t direction;
    Sense_t sense;
    uint8_t seg_x[MAX_SNAKE_LEN];
    uint8_t seg_y[MAX_SNAKE_LEN];
}Snake_t;

typedef struct{
    uint8_t x;
    uint8_t y;
} Dot_t;

typedef struct {
    Snake_t snake;
    SnakeState_t state;
    uint8_t level;
    uint8_t gamescreen[MAX_LEVEL];
    uint8_t max_level;
    Dot_t food;
} SnakeScreen_t;

static void SnakeRender(SnakeScreen_t *g){
    for(uint8_t r=0;r<MAX_LEVEL;++r) g->gamescreen[r]=0;
    for(uint8_t i=0;i<g->snake.length;++i){
        uint8_t x = g->snake.seg_x[i] & 0x7;
        uint8_t y = g->snake.seg_y[i] & 0x7;
        g->gamescreen[y] |= (uint8_t)(1u<<x);
    }
    uint8_t food_x = g->food.x & 0x7;
    uint8_t food_y = g->food.y & 0x7;
    g->gamescreen[food_y] |= (uint8_t)(1u<<food_x);
    load_output(GetMatrix(), g->gamescreen);
}

static void SnakeAdvance(SnakeScreen_t *g){
    Snake_t *s = &g->snake;

    // Compute new head
    int nx = s->head_x;
    int ny = s->head_y;
    if(s->direction == VERTICAL){
        ny += (s->sense == POSITIVE) ? 1 : -1;
    } else {
        nx += (s->sense == POSITIVE) ? 1 : -1;
    }
    nx = (nx + 8) % 8;
    ny = (ny + 8) % 8;

    // Shift body backwards
    for(int i = s->length - 1; i > 0; --i){
        s->seg_x[i] = s->seg_x[i-1];
        s->seg_y[i] = s->seg_y[i-1];
    }

    // Insert new head
    s->seg_x[0] = (uint8_t)nx;
    s->seg_y[0] = (uint8_t)ny;
    s->head_x = (uint8_t)nx;
    s->head_y = (uint8_t)ny;
}

static void SnakeGrow(SnakeScreen_t *g){
    Snake_t *s = &g->snake;
    if(s->length < MAX_SNAKE_LEN){
        // Duplicate last segment to extend tail
        s->seg_x[s->length] = s->seg_x[s->length - 1];
        s->seg_y[s->length] = s->seg_y[s->length - 1];
        s->length++;
    }
}

static void NewFood(SnakeScreen_t *g){
    // Simple random placement, no collision checking for brevity
    srand(xTaskGetTickCount());
    g->food.x = rand() % 8;
    g->food.y = rand() % 8;
}

static int CheckFoodCollision(SnakeScreen_t *g){
    Snake_t *s = &g->snake;
    
    return (s->head_x == g->food.x && s->head_y == g->food.y);
}

Snake_t SnakeInit(){
    Snake_t new_snake;
    new_snake.length = SNAKE_INITIAL_LEN;
    new_snake.head_x = 4;
    new_snake.head_y = 4;
    new_snake.direction = VERTICAL;
    new_snake.sense = POSITIVE;
    // Initialize straight vertical body
    for(uint8_t i=0;i<new_snake.length;++i){
        new_snake.seg_x[i] = new_snake.head_x;
        new_snake.seg_y[i] = (uint8_t)((new_snake.head_y - i + 8)%8);
    }
    return new_snake;
}

SnakeScreen_t SnakeGameInit(){
    SnakeScreen_t new_snake_game;
    new_snake_game.snake = SnakeInit();
    new_snake_game.state = SNAKE_STALL;
    new_snake_game.level = 1;
    for(size_t i=0;i<MAX_LEVEL;++i){
        new_snake_game.gamescreen[i] = 0x00;
    }
    new_snake_game.max_level = MAX_LEVEL;

    // Place food at a fixed position for simplicity
    NewFood(&new_snake_game);
    return new_snake_game;
}

Button sw2,sw3;
SnakeScreen_t snake_game;

void ButtonHandler(const Button *btn, ButtonEvent_t event){
    switch(event) {
        case BUTTON_EVENT_PRESS:
            if (snake_game.state == SNAKE_STALL) {
                snake_game = SnakeGameInit();
                snake_game.state = SNAKE_MOVING;
            }
            else {
                if(snake_game.snake.sense^snake_game.snake.direction){
                    snake_game.snake.sense = (btn == &sw2) ? NEGATIVE : POSITIVE;
                } else {
                    snake_game.snake.sense = (btn == &sw2) ? POSITIVE : NEGATIVE;
                }
                snake_game.snake.direction = (snake_game.snake.direction == VERTICAL) ? HORIZONTAL : VERTICAL;
            }
            break;
        default:
            break;
    }
}


void vSnakeTask(void *pvParameters){
    for(;;){
        switch(snake_game.state){
            case SNAKE_MOVING:
                if(CheckFoodCollision(&snake_game)){
                    SEGGER_RTT_WriteString(0, "Food eaten!\n");
                    snake_game.state = SNAKE_GROWING;
                    NewFood(&snake_game);
                }
                SnakeAdvance(&snake_game);
                SnakeRender(&snake_game);
                break;
            case SNAKE_GROWING:
                SnakeAdvance(&snake_game);
                SnakeGrow(&snake_game);
                SnakeRender(&snake_game);
                snake_game.state = SNAKE_MOVING;
                break;
            case SNAKE_IDLE:
            case SNAKE_GAMEOVER:
            case SNAKE_STALL:
            default:
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(SPEED));
    }
}

void RunApp(void)
{
    snake_game = SnakeGameInit();

    // Placeholder for Snake game logic
    SEGGER_RTT_WriteString(0, "Snake game started.\n");
    // Game loop and logic would go here
    xTaskCreate(vSnakeTask, "SnakeTask", 256, NULL, tskIDLE_PRIORITY + 1, NULL);
    Button_Init(&sw2, BUTTON_GPIO_Port, SW2_Pin, ButtonHandler);
    Button_Init(&sw3, BUTTON_GPIO_Port, SW3_Pin, ButtonHandler);

}