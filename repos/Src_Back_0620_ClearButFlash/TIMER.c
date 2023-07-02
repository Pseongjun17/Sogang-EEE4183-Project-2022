#include "TIMER.h"

#define MAX_NUM_FUNC_PTR (20)
static void (*fp_list[MAX_NUM_FUNC_PTR])(void);
static uint32_t tim2_func_intervals[MAX_NUM_FUNC_PTR];
static uint32_t tim2_func_timer[MAX_NUM_FUNC_PTR];

static time_t *timer_ptr = NULL;
static int64_t Total_Elapsed_Sec;
static int64_t Total_Elapsed_Tick;


static int clock_running;

// Global set-up
void init_timer(void)
{
    Total_Elapsed_Sec = 0;
    Total_Elapsed_Tick = 0;
    clock_running = 0;
	
    RCC->APB2ENR |= 1 << 11;
    TIM1->CR1 = 0x04;                           // only overflow of counter will be invoke interrupt
    TIM1->CR2 = 0x00;                           // Default set-up
    TIM1->PSC = 7400000000/1000 - 1;                     // 2ms scale
    TIM1->ARR = 1000 - 1;                      // interrupt per 1 second
    TIM1->DIER = 0x0001;                        // interrupt only when update
    NVIC->ISER[0] = NVIC->ISER[0] | 0x02000000; // turn on interrupt
    TIM1->CR1 = TIM1->CR1 | 0x01;

    RCC->APB1ENR |= 1 << 0;
    TIM2->CR1 = 0x04;
    TIM2->CR2 = 0x00; // default setup
    TIM2->PSC = 24000000/1000 - 1;
    TIM2->ARR = 10 - 1; // overflow for each 10ms
    TIM2->DIER = 0x0001;
    NVIC->ISER[0] = NVIC->ISER[0] | (1 << 28); // turn on interrupt
    TIM2->CR1 = TIM2->CR1 | 0x01;

    RCC->APB1ENR |= 1 << 1;
    TIM3->CR1 = 0x04;
    TIM3->CR2 = 0x00;
    TIM3->PSC = 24000000/1000 - 1; // 2ms scale
    // TIM3->ARR =  // SET NEXT~
    NVIC->ISER[0] = NVIC->ISER[0] | (1 << 29); // turn on interrupt
    TIM3->DIER = 0x0001;

    RCC->APB1ENR |= 1 << 2;
    TIM4->CR1 = 0x04;
    TIM4->CR2 = 0x00;       // default setup
    TIM4->PSC = 24000000/2000 - 1; // 2ms scale
    // TIM4->ARR = // see next~
    TIM4->DIER = 0x0001;

    NVIC->ISER[0] = NVIC->ISER[0] | (1 << 30); // turn on interrupt

}


//////////////////
// TIMER 1
//////////////////
void run_clock(void)
{
    clock_running = 1;
}

void stop_clock(void)
{
    clock_running = 0;
}

void set_second_pointer(time_t *time_data)
{
    timer_ptr = time_data;
	timer_ptr->hour = 12;
	timer_ptr->is_24H = 0;
	timer_ptr->min = 0;
	timer_ptr->sec = 0;
}

void TIM1_UP_IRQHandler(void)
{
    if ((TIM1->SR & 0x0001) != 0)
    {
        if (timer_ptr != NULL && clock_running == 1)
        {
            timer_ptr->sec++;
					
					
           	timer_ptr->min += timer_ptr->sec / 60;
            timer_ptr->hour += timer_ptr->min / 60;

            timer_ptr->hour = timer_ptr->hour % 24;
            timer_ptr->min = timer_ptr->min % 60;
            timer_ptr->sec = timer_ptr->sec % 60;
        }
        TIM1->SR &= ~(1 << 0); // clear UIF
    }

    Total_Elapsed_Sec++;

    return;
}

int now_time(void)
{
    return Total_Elapsed_Sec;
}

int64_t now_tick(void)
{
    return Total_Elapsed_Tick;
}


//////////////////
// TIMER 2
//////////////////
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

void no_more_function_call(void (*fp)(void))
{
    assert(fp != NULL);

    int i;
    for (i = 0; i < MAX_NUM_FUNC_PTR; i++)
    {
        if (fp_list[i] == fp)
        {
            fp_list[i] = NULL;
            break;
        }
    }
//    assert(i < MAX_NUM_FUNC_PTR); // when given function ptr is nto exists
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
										tim2_func_timer[i] = 0;
                }
                tim2_func_timer[i]++;
            }
        }
        TIM2->SR &= ~(1 << 0); // clear UIF
    }
		Total_Elapsed_Tick++;
    
}



//////////////////
// TIMER 3
//////////////////
void fast_interrupt_setup(int time_in_ms)
{
    uint32_t ARR = time_in_ms / 2;
    TIM3->ARR = ARR;

    TIM3->CR1 = TIM3->CR1 | 1;
}

void fast_interrupt_resume(void)
{
    TIM3->CR1 = TIM3->CR1 | 1;
}

void fast_interrupt_pause(void)
{
    TIM3->CR1 = TIM3->CR1 & ~1;
}




//////////////////
// TIMER 4
//////////////////

void fast_interrupt2_setup(int time_in_ms)
{
    uint32_t ARR = time_in_ms / 2;
    TIM4->ARR = ARR;

    TIM4->CR1 = TIM4->CR1 | 1;
}

void fast_interrupt2_resume(void)
{
    TIM4->CR1 = TIM4->CR1 | 1;
}

void fast_interrupt2_pause(void)
{
    TIM4->CR1 = TIM4->CR1 & ~1;
}

