#include "snake.h"
#include "common.h"

#define SPEED 500
#define MAX_LEVEL 8

typedef enum {
    HORIZONTAL,
    VERTICAL
}Direction_t;

typedef enum{
    POSITIVE,
    NEGATIVE
}Sense_t;

typedef enum {
    SNAKE_IDLE,
    SNAKE_MOVING,
    SNAKE_GROWING,
    SNAKE_GAMEOVER,
    SNAKE_STALL
}SnakeState_t;

typedef struct{
    uint8_t x;
    uint8_t y;
    uint8_t length;
    Direction_t direction;
    Sense_t sense;
}Snake_t;

typedef struct {
    Snake_t snake;
    SnakeState_t state;
    uint8_t level;
    uint8_t gamescreen[MAX_LEVEL];
    uint8_t max_level;
} SnakeScreen_t;

Snake_t SnakeInit(){
    Snake_t new_snake;
    new_snake.x = 4;
    new_snake.y = 4;
    new_snake.length = 1;
    new_snake.direction = VERTICAL;
    new_snake.sense = POSITIVE;
    return new_snake;
}

SnakeScreen_t SnakeGameInit(){
    SnakeScreen_t new_snake_game;
    new_snake_game.snake = SnakeInit();
    new_snake_game.state = SNAKE_MOVING;
    new_snake_game.level = 1;
    for(size_t i=0;i<MAX_LEVEL;++i){
        new_snake_game.gamescreen[i] = 0x00;
    }
    new_snake_game.gamescreen[new_snake_game.snake.y] = (1 << new_snake_game.snake.x);
    new_snake_game.max_level = MAX_LEVEL;
    return new_snake_game;
}

SnakeScreen_t snake_game;

void vSnakeTask(void *pvParameters){
    // Placeholder for Snake game task
    for(;;){
        switch(snake_game.state){
            case SNAKE_IDLE:
                // Handle idle state
                break;
            case SNAKE_MOVING:
                if (snake_game.snake.direction)
                {
                    // Vertical movement
                    if (snake_game.snake.sense == POSITIVE)
                        snake_game.snake.y = (snake_game.snake.y + 1) % 8;
                    else
                        snake_game.snake.y = (snake_game.snake.y - 1 + 8) % 8;
                }
                else
                {
                    // Horizontal movement
                    if (snake_game.snake.sense == POSITIVE)
                        snake_game.snake.x = (snake_game.snake.x + 1) % 8;
                    else
                        snake_game.snake.x = (snake_game.snake.x - 1 + 8) % 8;
                }
                snake_game.gamescreen[snake_game.snake.y] |= (1 << snake_game.snake.x);
                load_output(GetMatrix(), snake_game.gamescreen);
                break;
            case SNAKE_GROWING:
                // Handle growing state
                break;
            case SNAKE_GAMEOVER:
                // Handle game over state
                break;
            case SNAKE_STALL:
                // Handle stall state
                break;
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

}