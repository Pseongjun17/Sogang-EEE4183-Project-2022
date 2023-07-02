#ifndef _MAIN_H_
#define _MAIN_H_

#include <stm32f10x.h>
#include "ADC.h"
#include "DOT_MATRIX.h"
#include "GPIO.h"
#include "KEY_MATRIX.h"
#include "TIMER.h"
#include "USART.h"


///////
//      Pin Alloc
//      Dot Matrix : PB0~PB16 : col && PC0~PC7  : ROW
//      TMP : PA4
//      Key Matrix : PC0~PC8
//      Board KED : PA5
//      Switch  :  PC13  ??
//		USART : PA9, PA10


int32_t RecentCommand;
bool CommandRecieved;


extern int32_t RecentCommand;
extern bool CommandRecieved;

#ifdef RX_BOARD
enum StateEnum
{
	Idle,
	TimeSet,
	TimePrint,
	CelsiTempPrint,
	FerenTempPrint
};

enum StateEnum BoardState;

void StateMachine();
void print_time();
#endif

#ifdef TX_BOARD


void key_in_send();
void Temp_send();

#endif



time_t main_timer;
DotMatrix_t main_display;
KeyMatrix_t main_key;
int32_t TempVal;






#endif