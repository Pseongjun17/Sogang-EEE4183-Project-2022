#include <stm32f10x.h>
#include "ADC.h"
#include "DOT_MATRIX.h"
#include "GPIO.h"
#include "KEY_MATRIX.h"
#include "TIMER.h"
#include "USART.h"

int main()
{
	init_timer();
	init_dot_display(&main_display, PB8, PC0);

	while (1)
		;

	return 0;
}
