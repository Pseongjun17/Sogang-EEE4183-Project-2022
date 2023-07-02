#ifndef _DOT_MATRIX_H_
#define _DOT_MATRIX_H_

#include <stm32f10x.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "GPIO.h"

#define ROW_SCAN_INTERVAL   (2)
#define DOT_MAT_ROW_SIZE   (8)
#define DOT_MAT_COL_SIZE   (8)
#define DOT_MAT_CHAR_SIZE   (8)

#define FLASH_INTERVAL     (500)
#define rolling_speed  (1000)


//extren const u8 font8x8;

typedef struct DotMatrix_st
{
    GPIO_PORT_t row_port;
    GPIO_PORT_t col_port;
    int turned_on_row;
    u8 output_buf[DOT_MAT_ROW_SIZE];


    char *string_buf;
    int str_length;
    u8 next_pattern[DOT_MAT_ROW_SIZE];
    bool invalid_next_pattern;
    int current_str_cursor;
    int rolling_seq;

    bool is_rotated;
    // 0 means row is horizontally side (part num is on south)
    // rotate to vertical is bone by clock wise

    bool is_rolling;
    int rolling_position;
    int rolling_delay; // used for double matrix
    int rolling_time_interval; //(ms)

    // bool is_blinking;
    bool blinking_on;


    bool is_major_changed;
}DotMatrix_t;


extern DotMatrix_t main_display;

// input row / col is first pin name of them. 
// all pin of row / col MUST be continuous
void init_dot_display(DotMatrix_t *display, enum GPIO_pin_number row, enum GPIO_pin_number col);
// void setup_rolling(DotMatrix_t *display);

void print_string_display(DotMatrix_t *display, char* str);
void rotate_display(DotMatrix_t *display);




void claer_output_buf(DotMatrix_t *display);


void turn_on_display(DotMatrix_t *display);
void turn_off_display(DotMatrix_t *display);

void rolling_dot_mat(void);
void flash_display(void);

#endif


