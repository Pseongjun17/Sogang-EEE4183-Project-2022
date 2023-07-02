#include "DOT_MATRIX.h"
#include "DotMatrixPattern.h"


static DotMatrix_t *interrupt_display_ptr;


void init_dot_display(DotMatrix_t *display, enum GPIO_pin_number row, enum GPIO_pin_number col)
{
    init_GPIO_pin(&display->row_port, PushPullOut, row, DOT_MAT_ROW_SIZE);
    init_GPIO_pin(&display->col_port, PushPullOut, col, DOT_MAT_COL_SIZE);

    display->turned_on_row = 0;

    display->str_length = 0;
    display->string_buf = NULL;

    display->is_rotated = 0;
    display->is_rolling = 0;

    
    // display->is_blinking = 0;
    display->blinking_on = 1;
    
    
    
    display->is_major_changed = 0;

    fast_interrupt_setup(ROW_SCAN_INTERVAL);
		claer_output_buf(display);

    interrupt_display_ptr = display;
}


void print_string_display(DotMatrix_t *display, char* str)
{
    if(strlen(str) != display->str_length - 1)
    {
        if(display->string_buf != NULL)
            free(display->string_buf);
        
        display->string_buf = (char*)malloc( (1 + 1 + strlen(str)) * sizeof(char) ); // 1 for last blank, 1 for '\0'
        display->str_length = strlen(str) + 1;

        claer_output_buf(display);
        turn_on_display(display);
    }

    strcpy(display->string_buf, str);
    display->string_buf[display->str_length - 1] = ' ';
    display->string_buf[display->str_length ] = '\0';
}

// User Level
///////////////////////////////////////////////////////////
// inner level

void turn_on_display(DotMatrix_t *display)
{
    // for(int i = 0 ; i < DOT_MAT_ROW_SIZE; i++)
    // {
    //     display->output_buf[i] = font8x8[ display->string_buf[0] ][i];
    // }
    display->current_str_cursor = 0; //
    display->rolling_seq = 0;
    display->invalid_next_pattern = 1; 
    request_function_call(rolling_dot_mat, rolling_speed);
}

void turn_off_display(DotMatrix_t *display)
{
    claer_output_buf(display);
    no_more_function_call(rolling_dot_mat);
}


void claer_output_buf(DotMatrix_t *display)
{
    for(int i = 0 ; i < DOT_MAT_ROW_SIZE; i++)
    {
        display->output_buf[i] = 0;
    }
    return;
}


////////////////// Periodically calling function ////////////////////////

void flash_display(void)
{
    if(main_display.blinking_on)
    {
        main_display.blinking_on = 0;
    }
    else
    {
        main_display.blinking_on = 1;
    }
}

 

void rolling_dot_mat(void)
{
    int current_str_idx = main_display.rolling_seq / DOT_MAT_CHAR_SIZE;
    int roll_in_char = main_display.rolling_seq % DOT_MAT_CHAR_SIZE;

    uint16_t output_non_rolled[8];

    for(int i = 0; i < DOT_MAT_ROW_SIZE; i++)
    {
        char char1 = (current_str_idx-1 < 0)? ' ' : main_display.string_buf[current_str_idx-1];
        char char2 = main_display.string_buf[current_str_idx];

        output_non_rolled[i] = ( font8x8[(uint16_t)char1][i] << DOT_MAT_COL_SIZE) |  font8x8[(uint16_t)char2][i];

        output_non_rolled[i] = output_non_rolled[i] >> (7 - roll_in_char);
        main_display.output_buf[i] = (u8)(output_non_rolled[i] & 0b11111111);
    }


    main_display.rolling_seq++;

    if(main_display.rolling_seq == DOT_MAT_CHAR_SIZE * main_display.str_length)
        main_display.rolling_seq = 0;
}

//////////////////////row scan////////////////////
void TIM3_IRQHandler(void)
{
    if ((TIM3->SR & 0x0001) != 0)
	{
        int mask;   // add next : to column output mask

			write_val_to_GPIO(&(main_display.row_port), (~(1 << main_display.turned_on_row)) & 0xFF);
			write_val_to_GPIO(&(main_display.col_port), main_display.output_buf[main_display.turned_on_row] );

			main_display.turned_on_row++;
			if(main_display.turned_on_row == DOT_MAT_ROW_SIZE)
					main_display.turned_on_row = 0;
			
			
			if( main_display.blinking_on == 0)
				write_val_to_GPIO(&(main_display.row_port), 0xFF);
				
	}
	TIM3->SR &= ~(1 << 0); // clear UIF	
}












/////////LEGACY
    // if(main_display.invalid_next_pattern == 1)
    // {
    //     char next_char = main_display.string_buf[ main_display.current_str_cursor + 1 ] ;
    //     for(int i = 0 ; i < DOT_MAT_ROW_SIZE; i++)
    //     {
    //         main_display.next_pattern[i] = font8x8[next_char][i];
    //     }
	// 			main_display.invalid_next_pattern = 0;
    // }

    // for(int i = 0 ; i < DOT_MAT_ROW_SIZE; i++)
    // {
    //     main_display.output_buf[i] = main_display.output_buf[i] << 1;
			
    //     int new_col = (main_display.next_pattern[i] & 0b10000000) >> 7;
    //     main_display.output_buf[i] = main_display.output_buf[i] | new_col;

    //     main_display.next_pattern[i] = main_display.next_pattern[i] << 1;
    // }

    // main_display.rolling_seq++;
    // if(main_display.rolling_seq == DOT_MAT_CHAR_SIZE)
    // {
    //     main_display.rolling_seq = 0;

    //     main_display.current_str_cursor++;
    //     main_display.invalid_next_pattern = 1;
    // }
    // if(main_display.current_str_cursor == main_display.str_length - 1)
    // {
    //     main_display.current_str_cursor = -1;
    // }