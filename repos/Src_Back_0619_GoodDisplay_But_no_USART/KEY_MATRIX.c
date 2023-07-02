#include "KEY_MATRIX.h"



// input row / col is first pin name of them. 
// all pin of row / col MUST be continuous
void init_key_matrix(KeyMatrix_t *key_mat, enum GPIO_pin_number row, enum GPIO_pin_number col)
{
    init_GPIO_pin(&key_mat->row_port, PushPullOut, row, KEY_MAT_ROW_SIZE);
    init_GPIO_pin(&key_mat->col_port, InputPullDown, col, KEY_MAT_COL_SIZE);

    key_mat->scanning_row = 0;

    key_mat->lastest_value = INVALID_KEY_MAT_INPUT;
    key_mat->is_value_readed = 0;

    for(int i = 0 ; i < KEY_MAT_ROW_SIZE; i++)
    {
        key_mat->past_key_status[i] = 0b0000;
    }

    request_function_call(key_scan, KEY_SCAN_INTERVAL);
}


// data fetch
int get_lastest_input_KeyMat(KeyMatrix_t *key_mat)
{
    if(key_mat->is_value_readed)
    {
        return INVALID_KEY_MAT_INPUT;
    }
    else
    {
        key_mat->is_value_readed = 1;
        return key_mat->lastest_value;
    }
}



// key scanning
void key_scan(void)
{
	
    write_val_to_GPIO(&(main_key.row_port), (1 << main_key.scanning_row) & 0xF);
  //  sleep(KEY_SCAN_INTERVAL/2);
		for(int i = 0 ; i< 1000; i++);
	
    int column_key_stat = get_val_from_GPIO(&(main_key.col_port));

    if (column_key_stat != main_key.past_key_status[main_key.scanning_row])
    {
        for(int col_num = 0; col_num < KEY_MAT_COL_SIZE; col_num ++)
        {
            if( (main_key.past_key_status[main_key.scanning_row] & (1<<col_num) ) == 0) // past released
            {
                if((column_key_stat & (1<<col_num)) != 0)   // BUT now pushed
                {
                    main_key.lastest_value = main_key.scanning_row * KEY_MAT_COL_SIZE + col_num;
                    main_key.is_value_readed = 0;
                }
            }
        }
    }
    main_key.past_key_status[main_key.scanning_row] = column_key_stat;

    main_key.scanning_row++;
    if(main_key.scanning_row == KEY_MAT_ROW_SIZE)
        main_key.scanning_row = 0;
}
