#include "main.h"

int main()
{
//////////////System Init///////////////	
	init_timer();
	init_dot_pattern();
	set_second_pointer(&main_timer);
	init_USART( &main_usart, 1, PA5);
	set_up_monitoring_master( &main_usart );
//////////////////////////////////////////
	
// //////////////Varibles Init///////////////	
// 	TempVal = 0;
// #ifdef TX_BOARD

// #endif
// #ifdef RX_BOARD
// 	BoardState = Idle;
// 	RecentCommand = 0;
// 	CommandRecieved = false;
// #endif
// //////////////////////////////////////////

	
////////////// HardWare init /////////////
#ifdef RX_BOARD
//	init_dot_display(&main_display, PC0, PB0);
//	print_string_display(&main_display, "1234'5678");
#endif
#ifdef TX_BOARD
	init_key_matrix(&main_key, PC4, PC0);
#endif

//////////////////////////////////////////
	
	
//////////// Request Functions ////////////
#ifdef TX_BOARD
	request_function_call(key_in_send, 100);
	request_function_call(Temp_send, 1000);
#endif

#ifdef RX_BOARD
//	request_function_call(StateMachine, 300);
#endif
//////////////////////////////////////////


	while(1)
	{
		__WFI();
	}
	return 0;
}







#ifdef TX_BOARD
void key_in_send()
{
	int key_in = get_lastest_input_KeyMat(&main_key);
	if(key_in != INVALID_KEY_MAT_INPUT)
		send_message(&main_usart, Command, key_in);
}
void Temp_send()
{
	send_message(&main_usart, Temp_Send, TempVal);
}
#endif



#ifdef RX_BOARD
void StateMachine()
{
	switch(BoardState)
	{
	case Idle:

		break;
	////////////////////////////////////////
	case TimeSet:

		break;
	////////////////////////////////////////
	case TimePrint:

		break;
	////////////////////////////////////////
	case CelsiTempPrint:
	
		break;
	////////////////////////////////////////
	case FerenTempPrint:
	
		break;
	////////////////////////////////////////
	default:
		break;
	}
}


void print_time()
{
	char time_str[9];
	

	time_str[0] = main_timer.hour / 10 + '0';
	time_str[1] = main_timer.hour % 10 + '0';
	
	time_str[2] = ':';

	time_str[3] = main_timer.min / 10 + '0';
	time_str[4] = main_timer.min % 10 + '0';
	
	time_str[5] = ':';

	time_str[6] = main_timer.sec / 10 + '0';
	time_str[7] = main_timer.sec % 10 + '0';

	time_str[8] = '\0';

	print_string_display(&main_display, time_str);
}
#endif