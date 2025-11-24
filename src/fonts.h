#ifndef __FONTS_H
#define __FONTS_H

#include <stdint.h>
#include "string.h"

extern uint8_t digits[10][8];
extern uint8_t letters[69][8];

int get_char(char c);
void scroll_text(char* text, uint8_t speed);
void load_char_to_screen(char* c, uint8_t speed);

#endif