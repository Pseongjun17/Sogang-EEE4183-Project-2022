#ifndef _KEY_MATRIX_H_
#define _KEY_MATRIX_H_

#include <stm32f10x.h>
#include <stdbool.h>
#include "GPIO.h"
#include "TIMER.h"


#define KEY_SCAN_INTERVAL   (52)
#define KEY_MAT_ROW_SIZE   (4)
#define KEY_MAT_COL_SIZE   (4)

#define INVALID_KEY_MAT_INPUT (-1)


typedef struct KeyMatrix_st
{
    GPIO_PORT_t row_port;
    GPIO_PORT_t col_port;
    int scanning_row;

    int lastest_value;
    bool is_value_readed;

    char past_key_status[KEY_MAT_ROW_SIZE];

}KeyMatrix_t;

extern KeyMatrix_t main_key;


// input row / col is first pin name of them. 
// all pin of row / col MUST be continuous
void init_key_matrix(KeyMatrix_t *key_mat, enum GPIO_pin_number row, enum GPIO_pin_number col);

// data fetch
int get_lastest_input_KeyMat(KeyMatrix_t *key_mat);

// key scanning
void key_scan(void);


#endif
