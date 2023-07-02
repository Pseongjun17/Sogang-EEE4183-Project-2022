#include "main.h"

int64_t temp_tick;

int32_t RecentCommand;
bool CommandRecieved;
int TempVal;
bool is_Celci;

#ifdef RX_BOARD
static void boot_up();
static void store_data();
#endif

int main()
{
//////////////System Init///////////////	
	init_timer();
	init_dot_pattern();
	set_second_pointer(&main_timer);
	init_USART( &main_usart, 1, PA5);
	set_up_monitoring_master( &main_usart );
//////////////////////////////////////////
	
//////////////Varibles Init///////////////	
TempVal	 = 0;
#ifdef TX_BOARD

#endif
#ifdef RX_BOARD
	is_Celci = 1;
	BoardState = Idle;
	RecentCommand = 0;
	boot_up();
	CommandRecieved = false;
#endif
//////////////////////////////////////////

	
////////////// HardWare init /////////////
#ifdef TX_BOARD
	init_key_matrix(&main_key, PC4, PC0);
	init_temperature_ADC(&TempVal);
#endif

#ifdef RX_BOARD
	init_dot_display(&main_display, PC0, PB0);	
#endif
//////////////////////////////////////////
	
	
//////////// Request Functions ////////////
#ifdef TX_BOARD
	request_function_call(key_in_send, 100);
	request_function_call(Temp_send, 1000);
#endif

#ifdef RX_BOARD
	print_string_display(&main_display, " ");
	//request_function_call(print_temp, 300);
	stop_clock();
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
	static enum StateEnum pre_state;

	switch(BoardState)
	{
	case Idle:
		if(CommandRecieved == true && RecentCommand == 1)
		{
			CommandRecieved = false;
			BoardState = TimeSet;
			
			request_function_call(print_time, 300);
			flash_toggle(&main_display, 1);
			stop_clock();

			request_function_call(StateMachine, ONE_SEC);
		}
		else
		{
			
		}
		break;
	////////////////////////////////////////
	case TimeSet:
		if(CommandRecieved == false) // Called by timer interrupt (1 sec pasted)
		{
			BoardState = TimePrint;
			flash_toggle(&main_display, 0);
			run_clock();

			no_more_function_call(StateMachine);	
		}
		else
		{
			CommandRecieved = false;
			if(RecentCommand == 4)
				main_timer.is_24H = 1;
			if(RecentCommand == 5)
				main_timer.is_24H = 0;
			if(RecentCommand == 6)
				main_timer.hour++;
			if(RecentCommand == 7)
				main_timer.hour--;
			if(RecentCommand == 8)
				main_timer.min++;
			if(RecentCommand == 9)
				main_timer.min--;
			if(RecentCommand == 10)
				main_timer.sec++;
			if(RecentCommand == 11)
				main_timer.sec--;
			
			main_timer.hour = (main_timer.hour + 24) % 24;
			main_timer.min = (main_timer.min + 60) % 60;
			main_timer.sec = (main_timer.sec + 60) % 60;

			store_data();

			no_more_function_call(StateMachine);
			request_function_call(StateMachine, ONE_SEC);
			
			CommandRecieved = false;
		}
		break;
	////////////////////////////////////////
	case TimePrint:
		if(CommandRecieved == true && RecentCommand == 0)
		{
			CommandRecieved = false;
			BoardState = TempPrint;

			no_more_function_call(print_time);
			request_function_call(print_temp, 300);
		}
		if(CommandRecieved == true && RecentCommand == 3)
		{
			CommandRecieved = false;
			BoardState = TimeSet;

			flash_toggle(&main_display, 1);
			request_function_call(StateMachine, ONE_SEC);
		}
		if(CommandRecieved == true && RecentCommand == 5)
		{
			CommandRecieved = false;
			BoardState = ScrollSetting;

			flash_toggle(&main_display, 1);
			pre_state = TimePrint;
			request_function_call(StateMachine, ONE_SEC);
		}
		break;
	////////////////////////////////////////
	case TempPrint:
		if(CommandRecieved == true && RecentCommand == 0)
		{
			CommandRecieved = false;

			BoardState = TimePrint;
			
			
			no_more_function_call(print_temp);
			request_function_call(print_time, 300);
		}
		if(CommandRecieved == true && RecentCommand == 5)
		{
			CommandRecieved = false;
			BoardState = ScrollSetting;

			flash_toggle(&main_display, 1);
			pre_state = TempPrint;
			request_function_call(StateMachine, ONE_SEC);
		}
		if(CommandRecieved == true && RecentCommand == 3)
		{
			CommandRecieved = false;
			is_Celci = !is_Celci;
			store_data();
		}
		
		break;
	////////////////////////////////////////
	case ScrollSetting:
		if(CommandRecieved == false)
		{
			BoardState = pre_state;
			flash_toggle(&main_display, 0);
			no_more_function_call(StateMachine);
		}
		else
		{
			CommandRecieved = false;

			if(RecentCommand == 4)
				reverse_rolling(&main_display, 1);
			if(RecentCommand == 5)
				reverse_rolling(&main_display, 0);
			if(RecentCommand == 6)
				rotate_display(&main_display, 1);
			if(RecentCommand == 7)
				rotate_display(&main_display, 0);
		
			store_data();


			no_more_function_call(StateMachine);
			request_function_call(StateMachine, ONE_SEC);
		}
		break;
	////////////////////////////////////////
	default:
		break;
	}
}


void print_time()
{
	char time_str[10];
	
	if(main_timer.is_24H == 1)
		sprintf(time_str, "%02d:%02d:%02d", main_timer.hour, main_timer.min, main_timer.sec);
	else
		if( main_timer.hour % 12 != 0)
			sprintf(time_str, "%02d:%02d:%02d", main_timer.hour % 12, main_timer.min, main_timer.sec);
		else
			sprintf(time_str, "12:%02d:%02d", main_timer.min, main_timer.sec);

	print_string_display(&main_display, time_str);
}


void print_temp()
{
	char temp_str[20];
	if(is_Celci)
		sprintf(temp_str, "%2d.%02d'C", TempVal/100, TempVal%100);
	else
		sprintf(temp_str, "%2d.%02d'F", (TempVal/100) * 2 + 32, (TempVal * 2)%100 );

	print_string_display(&main_display, temp_str);
}

//////////////flash control////////////
static void boot_up()
{
	char config = 0;
	// config = read_from_flash();

	is_Celci = config & 0b1;
	main_display.is_rolling_rev = (config & 0b10) >> 1;
	main_display.is_rotated = (config & 0b100) >> 2;
	main_timer.is_24H = (config & 0b1000) >> 3;

}
static void store_data()
{	
	char config = 0;
	config |= is_Celci;
	config |= main_display.is_rolling_rev << 1;
	config |= main_display.is_rotated << 2;
	config |= main_timer.is_24H << 3;

	// write_into_flash(config);
}



#endif