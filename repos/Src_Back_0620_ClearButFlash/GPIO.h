// 
// GPIO HEADER FILE
// 
// author : Park SeongJun
//
//
//  This GPIO Library's target is ARM CORTEX M3 STM103


#ifndef _GPIO_H_
#define _GPIO_H_

#include <stm32f10x.h>
#include <assert.h>
#include <stdbool.h>
#include "GlobalParameters.h"

enum GPIO_pin_number
{
    PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11, PA12, PA13, PA14, PA15,
    PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13, PB14, PB15,
    PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7, PC8, PC9, PC10, PC11, PC12, PC13, PC14, PC15,
    PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7, PD8, PD9, PD10, PD11, PD12, PD13, PD14, PD15,
    PE0, PE1, PE2, PE3, PE4, PE5, PE6, PE7, PE8, PE9, PE10, PE11, PE12, PE13, PE14, PE15,
    PF0, PF1, PF2, PF3, PF4, PF5, PF6, PF7, PF8, PF9, PF10, PF11, PF12, PF13, PF14, PF15,
    PG0, PG1, PG2, PG3, PG4, PG5, PG6, PG7, PG8, PG9, PG10, PG11, PG12, PG13, PG14, PG15
};

enum GPIO_pin_mode
{
    AnalogInput, InputFloating, InputPullDown, PushPullOut
};

typedef struct GPIO_PORT_st
{
    GPIO_TypeDef *BASE_GPIO_ADDR;
    int number_of_pins;
    enum GPIO_pin_mode mode;

	  int GPIO_first_pin_bit;
    char GPIO_char;	
}GPIO_PORT_t;



void init_GPIO_pin(GPIO_PORT_t *port_to_init, enum GPIO_pin_mode mode, enum GPIO_pin_number start_pin, int number_of_pins);

void write_val_to_GPIO(GPIO_PORT_t *target_port, int value);
uint16_t get_val_from_GPIO(GPIO_PORT_t *target_port);


void set_AFIO(GPIO_PORT_t *target_port);


#endif


