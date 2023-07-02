#ifndef _DOT_MATRIX_H_
#define _DOT_MATRIX_H_

#include <stm32f10x.h>
#include <stdbool.h>
#include "GPIO.h"

#define ROW_SCAN_INTERVAL   (5)
#define MAT_SIZE   (8)

//extren const u8 font8x8;

typedef struct DotMatrix_st
{
    GPIO_PORT_t row_port;
    GPIO_PORT_t col_port;
    int turned_on_row;
    u8 *output;

    u8 output_buf[8];
    char *string_buf;
    int str_length;

    bool is_rotated;
    // 0 means row is horizontally side (part num is on south)
    // rotate to vertical is bone by clock wise

    bool is_rolling;
    int rolling_position;
    int rolling_delay; // used for double matrix
    int rolling_time_interval; //(ms)

    bool is_blinking;

    u8 turn_off_Zeros[8];

    bool is_major_changed;
}DotMatrix_t;


extern DotMatrix_t main_display;

// input row / col is first pin name of them. 
// all pin of row / col MUST be continuous
void init_dot_display(DotMatrix_t *display, enum GPIO_pin_number row, enum GPIO_pin_number col);


void print_string_display(DotMatrix_t *display, char* str);

void setup_rolling(DotMatrix_t *display, bool is_rolling);

void turn_on_display(DotMatrix_t *display);
void turn_off_display(DotMatrix_t *display);


//void FAST_INTERRUPT_ISR

#endif


