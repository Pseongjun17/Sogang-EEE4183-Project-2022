#include <stm32f10x.h>
#define ADC1_DR_BASE	0x4001244c

u16 ADCConverted;
u16 Celcius;
u16 Fahrenheit;

void adc_temperature (void) 
{
/// adc setting
RCC->APB2ENR |= 0x00000005;
GPIOA->CRL &= 0xFFF0FFFF;

RCC->AHBENR |= 0x00000001;
DMA1_Channel1->CCR = 0x00003520;
DMA1_Channel1->CNDTR = 1;
DMA1_Channel1->CPAR = ADC1_DR_BASE;
DMA1_Channel1->CMAR = (u32)&ADCConverted;

RCC->APB2ENR |= 0x00000200;
ADC1->CR1 = 0x00000020;
ADC1->CR2 = 0x001E0102;
ADC1->SMPR2 = 0x00007000;
ADC1->SQR1 = 0x00000000;
ADC1->SQR2 = 0x00000000;
ADC1->SQR3 = 0x00000004;
NVIC->ISER[0] |= 1<<18;
	
//// adc start
DMA1_Channel1->CCR |= 0x00000001;
ADC1->CR2 |= 0x00000001;
ADC1->CR2 |= 0x00400000;
}


/// value conversion
void ADC1_2_IRQHandler(void)
{
		Celcius = (ADCConverted - 500) / 10 ;
		Fahrenheit = Celcius * (9 / 5) + 32 ;
}