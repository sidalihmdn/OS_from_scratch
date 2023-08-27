#pragma once

#define VIDEO_ADDRESS 0xb8000

#define MAX_ROW 25
#define MAX_COL 80

#define WHITE_ON_BLACK 0x0f

#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

#include "../includes/unit_types.h"
// declarations

void print_char(char attribute, int col, int row, char character);
void zprint(char *string);
void print_string(char *string, uint8_t x, uint8_t y);

void set_cursor(int offset);

void clean_screen();
void print_char_no_cursor(char attribute, int col, int row, char character);
