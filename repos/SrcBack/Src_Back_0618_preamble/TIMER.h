// 
//                               TIMER HEADER FILE
// 
// author : Park SeongJun
//
//
//  This TIMER Library's target is ARM CORTEX M3 STM103
//
//  which inclueds FOUR TIMERS
//  we use Each timers for diffrent purpose
//
//        TIMER NUM       |           Purpose            |            Usage example
//  ------------------------------------------------------------------------------------------
//          TIM1          |         Global Timer         |       accurate time for clock
//          TIM2          |    high tolarence interrupt  | User-time controll like rolling display
//          TIM3          |    low tolarence interrupt   |     high-speed interrupt invoke
//          TIM4          |     General purpose timer    |     dedicate to 'SLEEP' function
//
//
//
//  CONFIGURATIONS
//  
//      TIM1 : 
//          counting accurate time is second unit
//
//      TIM2 :
//          make interrupts for each 10ms (100Hz)
//          user can set 10 functions to be called when interrupt occures
//          this callings are implemented by function pointer
//          and user can set interval of calling each functions
//
//      TIM3 :
//          only one function can be handled by this timer's interrupt
//          configuration of this timer can be very specifially
//
//      TIM4 :
//          This timer is for "SLEEP" function.
//          after given time, it invokes interrput
//
//
//
#ifndef _TIEMR_H_
#define _TIEMR_H_

#include <stm32f10x.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>


typedef struct time_st
{
	uint32_t hour;
	uint32_t min;
	uint32_t sec;
	bool is_PM;
}time_t;


// Global set-up
void init_timer(void);

// TIMER 1
void run_clock(void);
void stop_clock(void);
void set_second_pointer(time_t *time_data);
int now_time(void);

// TIMER 2
//   static varibles : 
//        void (*fp_list[10])();
//        uint32_t tim2_func_intervals[10];
void request_function_call(void (*fp)(void), int interval_ms);
void no_more_function_call(void (*fp)(void));

// TIMER 3
void fast_interrupt_setup(int time_in_ms);
void fast_interrupt_resume(void);
void fast_interrupt_pause(void);


// TIMER 4
void sleep(int time_in_ms);




#endif


