#include "main.h"

time_t main_timer;
DotMatrix_t main_display;
int32_t temp_val;

char temp_str[2];


void print_time();
void print_key2dot();


int main()
 {
	 
	 init_timer();
	
	sleep(10000);
	init_USART( &main_usart, 1, PA5);
	//set_up_monitoring_master( &main_usart );
	
	monitoring_send();
	
	__WFI();
	 
	 while(1);
	return 0;
}








void print_key2dot()
{
	char input_str[2];
	input_str[1] = '\0';

	int key_in = get_lastest_input_KeyMat(&main_key);
	
	if(key_in == INVALID_KEY_MAT_INPUT)
		;//input_str[0] = ' '-3;
	else
	{
		
		input_str[0] = key_in + '0';
		print_string_display(&main_display, input_str);
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