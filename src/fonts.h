#ifndef __FONTS_H
#define __FONTS_H

#include <stdint.h>

extern uint8_t digits[10][8];
extern uint8_t letters[52][8];

void scroll(uint16_t screen[2][8]);
int get_char(char c);
void load_char_to_screen(char* c);              
#endif