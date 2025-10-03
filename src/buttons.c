#include "buttons.h"
#include "common.h"

#define DEBOUNCE_MS      30
#define SHORT_PRESS_MS   50
#define LONG_PRESS_MS    800
#define DOUBLE_TAP_MS    200

void vButtonTask(void *pvParameters);
void Button_Update(Button *btn, uint32_t now_ms);

void Button_Init(Button *btn, GPIO_TypeDef *port, uint16_t pin, ButtonCallback cb){
    GPIO_InitTypeDef GPIO_InitStruct;

    btn->port = port;
    btn->pin = pin;
    btn->active_level = 0;  // hardcoded due to hardware limitations
    btn->state = 0;
    btn->last_state = 0;
    btn->last_change = 0;
    btn->press_time = 0;
    btn->click_count = 0;

    btn->long_press_check = 0;    

    btn->callback = cb;

    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(port, &GPIO_InitStruct);
    
    xTaskCreate(vButtonTask, "ButtonTask", 128, btn, 1, &btn->task_handle);
}

void vButtonTask(void *pvParameters)
{
    Button *sw = (Button *)pvParameters;
    for(;;) {
        uint32_t now = xTaskGetTickCount();
        Button_Update(sw, now);
        vTaskDelay(pdMS_TO_TICKS(10)); // poll every 10ms
    }
}

void Button_Update(Button *btn, uint32_t now_ms){
    uint8_t raw = (HAL_GPIO_ReadPin(btn->port, btn->pin) == GPIO_PIN_SET) ? 1 : 0;
    uint8_t pressed = (raw == btn->active_level);

    // debounce
    if (pressed != btn->last_state) {
        btn->last_change = now_ms;
        btn->last_state = pressed;
    }

    if ((now_ms - btn->last_change) > DEBOUNCE_MS) {
        if (pressed != btn->state) {
            btn->state = pressed;

            if (btn->state) { // pressed
                btn->press_time = now_ms;
                btn->long_press_check = 0;
                if (btn->callback) btn->callback(btn, BUTTON_EVENT_PRESS);
            } else { // released
                btn->release_time = now_ms;
                if (btn->callback) btn->callback(btn, BUTTON_EVENT_RELEASE);

                uint32_t press_duration = btn->release_time - btn->press_time;
                if (press_duration >= LONG_PRESS_MS) {
                    // already handled in hold loop, do nothing
                } else if (press_duration >= SHORT_PRESS_MS) {
                    btn->click_count++;
                }
            }
        }
    }

    // long press detection
    if (btn->state && !btn->long_press_check) {
        if ((now_ms - btn->press_time) >= LONG_PRESS_MS) {
            btn->long_press_check = 1;
            if (btn->callback) btn->callback(btn, BUTTON_EVENT_LONG);
        }
    }

    // handle single vs double tap
    if (btn->click_count > 0) {
        if ((now_ms - btn->release_time) > DOUBLE_TAP_MS) {
            if (btn->click_count == 1) {
                if (btn->callback) btn->callback(btn, BUTTON_EVENT_SHORT);
            } else if (btn->click_count == 2) {
                if (btn->callback) btn->callback(btn, BUTTON_EVENT_DOUBLE);
            }
            btn->click_count = 0;
        }
    }
}
