#include "TIMER.h"

#define MAX_NUM_FUNC_PTR (10)
static void (*fp_list[MAX_NUM_FUNC_PTR])(void);
static uint32_t tim2_func_intervals[MAX_NUM_FUNC_PTR];
static uint32_t tim2_func_timer[MAX_NUM_FUNC_PTR];

static uint32_t *second_elpased_ptr = NULL;

// Global set-up
void init_timer(void)
{
    RCC->APB2ENR |= 1 << 11;
    TIM1->CR1 = 0x04;                           // only overflow of counter will be invoke interrupt
    TIM1->CR2 = 0x00;                           // Default set-up
    TIM1->PSC = 0x8CA0 - 1;                     // 2ms scale
    TIM1->ARR = 0x7D0 - 1;                      // interrupt per 1 second
    TIM1->DIER = 0x0001;                        // interrupt only when update
    NVIC->ISER[0] = NVIC->ISER[0] | 0x02000000; // turn on interrupt
    TIM1->CR1 = TIM1->CR1 | 0x01;

    RCC->APB1ENR |= 1 << 0;
    TIM2->CR1 = 0x04;
    TIM2->CR2 = 0x00; // default setup
    TIM2->PSC = 0x16AE;
    TIM2->ARR = 0x3E; // overflow for each 10ms
    TIM2->DIER = 0x0001;
    NVIC->ISER[0] = NVIC->ISER[0] | (1 << 28); // turn on interrupt
    TIM2->CR1 = TIM2->CR1 | 0x01;

    RCC->APB1ENR |= 1 << 1;
    TIM3->CR1 = 0x04;
    TIM3->CR2 = 0x00;
    TIM3->PSC = 0x8CA0 - 1; // 2ms scale
    // TIM3->ARR =  // SET NEXT~
    NVIC->ISER[0] = NVIC->ISER[0] | (1 << 29); // turn on interrupt
    TIM3->DIER = 0x0001;

    RCC->APB1ENR |= 1 << 2;
    TIM4->CR1 = 0x04;
    TIM4->CR2 = 0x00;       // default setup
    TIM4->PSC = 0x8CA0 - 1; // 2ms scale
    // TIM4->ARR = // see next~
    TIM4->DIER = 0x0001;
}

// TIMER 1
void run_clock(bool make_interrupt)
{
    TIM1->CR1 = TIM1->CR1 | 0x01;
}

void stop_clock(bool make_interrupt)
{
    TIM1->CR1 = TIM1->CR1 & (~0x01);
}

void set_second_pointer(uint32_t *second_elpased)
{
    second_elpased_ptr = second_elpased;
}

void TIM1_UP_IRQHandler(void)
{
    if ((TIM1->SR & 0x0001) != 0)
    {
        if (second_elpased_ptr != NULL)
        {
            *(second_elpased_ptr)++;
        }
        TIM1->SR &= ~(1 << 0); // clear UIF
    }
    return;
}

// TIMER 2
void request_function_call(void (*fp)(void), int interval_ms)
{
    assert(50 < interval_ms);

    int i;
    for (i = 0; i < MAX_NUM_FUNC_PTR; i++)
    {
        if (fp_list[i] == NULL)
        {
            fp_list[i] = fp;
            tim2_func_intervals[i] = interval_ms / 50;
            tim2_func_timer[i] = 0;
            break;
        }
    }
    assert(i < MAX_NUM_FUNC_PTR); // when list is full
}

void TIM2_IRQHandler(void)
{
    if ((TIM2->SR & 0x0001) != 0)
    {
        for (int i = 0; i < MAX_NUM_FUNC_PTR; i++)
        {
            if (fp_list[i] != NULL)
            {
                if (tim2_func_timer[i] == tim2_func_intervals[i])
                {
                    fp_list[i]();
                }
                tim2_func_timer[i]++;
            }
        }
        TIM2->SR &= ~(1 << 0); // clear UIF
    }
}

// TIMER 3
void fast_interrupt_setup(int time_in_ms)
{
    uint32_t ARR = time_in_ms / 2;
    TIM3->ARR = ARR;

    TIM3->CR1 = TIM3->CR1 | 1;
}

// TIMER 4
void sleep(int time_in_ms)
{
    uint32_t ARR = time_in_ms / 2;
    TIM4->ARR = ARR;

    TIM4->CR1 = TIM4->CR1 | 1;

    while ((TIM4->SR & 0x0001) != 0)
        ;

    TIM4->CR1 = TIM4->CR1 & ~1;
    TIM4->CNT = 0;

    return;
}
