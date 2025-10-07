#ifndef __BUTTONS_H
#define __BUTTONS_H

#include "common.h"

// Gesture definitions
typedef enum {
    BUTTON_EVENT_NONE = 0,
    BUTTON_EVENT_PRESS,       // raw press
    BUTTON_EVENT_RELEASE,     // raw release
    BUTTON_EVENT_SHORT,       // short press detected
    BUTTON_EVENT_LONG,        // long press detected
    BUTTON_EVENT_DOUBLE,      // double tap detected
    BUTTON_EVENT_COMBO        // combination detected (handled externally)
} ButtonEvent_t;

struct Button;
typedef void (*ButtonCallback)(const struct Button *btn, ButtonEvent_t event);

typedef struct Button
{
    GPIO_TypeDef *port;
    uint16_t pin;

    //states
    uint8_t active_level;
    uint8_t state;
    uint8_t last_state;

    //timing
    uint32_t last_change;
    uint32_t press_time;
    uint32_t release_time;
    uint8_t click_count;

    uint8_t long_press_check;

    ButtonCallback callback;
    TaskHandle_t task_handle;
    void *task;
}Button;

/**
 * @brief Initialize a button structure and starts its FreeRTOS task
 * 
 * @param btn declared Button to initialize
 * @param port button port
 * @param pin button pin
 * @param cb callback function to handle button events
 */
void Button_Init(Button *btn, GPIO_TypeDef *port, uint16_t pin, ButtonCallback cb);

#endif