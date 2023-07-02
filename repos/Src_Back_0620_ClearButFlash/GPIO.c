#include "GPIO.h"

void init_GPIO_pin(GPIO_PORT_t *port_to_init, enum GPIO_pin_mode mode, enum GPIO_pin_number start_pin, int number_of_pins)
{
    // identify pin number configurations
    port_to_init->GPIO_first_pin_bit = start_pin % 16;
    port_to_init->number_of_pins = number_of_pins;

    // a port must include only single GPIO's pins
    // or it makes error
    assert(port_to_init->GPIO_first_pin_bit + port_to_init->number_of_pins - 1 < 16);

    // set BASE GPIO ADDR
    switch (start_pin / 16)
    {
    case 0:
        port_to_init->BASE_GPIO_ADDR = GPIOA;
        port_to_init->GPIO_char = 'A';
        RCC->APB2ENR |= 1<<2;
        break;
    case 1:
        port_to_init->BASE_GPIO_ADDR = GPIOB;
        port_to_init->GPIO_char = 'B';
        RCC->APB2ENR |= 1<<3;
        break;
    case 2:
        port_to_init->BASE_GPIO_ADDR = GPIOC;
        port_to_init->GPIO_char = 'C';
        RCC->APB2ENR |= 1<<4;
        break;
    case 3:
        port_to_init->BASE_GPIO_ADDR = GPIOD;
        port_to_init->GPIO_char = 'D';
        RCC->APB2ENR |= 1<<5;
        break;
    case 4:
        port_to_init->BASE_GPIO_ADDR = GPIOE;
        port_to_init->GPIO_char = 'E';
        RCC->APB2ENR |= 1<<6;
        break;
    case 5:
        port_to_init->BASE_GPIO_ADDR = GPIOF;
        port_to_init->GPIO_char = 'F';
        RCC->APB2ENR |= 1<<7;
        break;
    case 6:
        port_to_init->BASE_GPIO_ADDR = GPIOG;
        port_to_init->GPIO_char = 'G';
        RCC->APB2ENR |= 1<<8;
        break;
    default:
        assert(0);
        break;
    }

    // set mode configuration
    int mode_binary_configuration;
    switch (mode)
    {
    case AnalogInput:
        mode_binary_configuration = 0b0000;
        break;
    case InputFloating:
        mode_binary_configuration = 0b0100;
        break;
    case InputPullDown:
        mode_binary_configuration = 0b1000;
        break;
    case PushPullOut:
        mode_binary_configuration = 0b0011;
        break;
    default:
        assert(0);
        break;
    }
	
    // wirte mode onto register
    for(int i = port_to_init->GPIO_first_pin_bit; i < port_to_init->GPIO_first_pin_bit + port_to_init->number_of_pins; i++)
    {
        if(i < 8)
        {
            uint32_t mode_num_shifted = (uint32_t) (mode_binary_configuration << (i * 4));
            uint32_t mask = ~(0b1111 << (i * 4));
            port_to_init->BASE_GPIO_ADDR->CRL = port_to_init->BASE_GPIO_ADDR->CRL & mask;
            port_to_init->BASE_GPIO_ADDR->CRL = port_to_init->BASE_GPIO_ADDR->CRL | (uint32_t)mode_num_shifted;
        }
        else
        {
            int mode_num_shifted = mode_binary_configuration << ((i - 8) * 4);
            uint32_t mask = ~(0b1111 << ((i - 8) * 4));
            port_to_init->BASE_GPIO_ADDR->CRH = port_to_init->BASE_GPIO_ADDR->CRH & mask;
            port_to_init->BASE_GPIO_ADDR->CRH = port_to_init->BASE_GPIO_ADDR->CRH | (uint32_t)mode_num_shifted;
        }
    }

}


void write_val_to_GPIO(GPIO_PORT_t *target_port, int value)
{
    int mask = (1 << (target_port->number_of_pins)) - 1;
    assert((value & ~mask) == 0);

    uint32_t inverted_value = ~value & mask;
    uint32_t BSRR_VAL = ( (inverted_value << (16 + target_port->GPIO_first_pin_bit)) + \
                        (value << target_port->GPIO_first_pin_bit) );
    
    target_port->BASE_GPIO_ADDR->BSRR = (uint32_t)BSRR_VAL;
}

uint16_t get_val_from_GPIO(GPIO_PORT_t *target_port)
{
    uint16_t mask = (1 << (uint16_t)(target_port->number_of_pins)) - 1;
    return (target_port->BASE_GPIO_ADDR->IDR >> target_port->GPIO_first_pin_bit) & mask;
}


void set_AFIO(GPIO_PORT_t *target_port)
{
    
}
