#include "USART.h"

USART_t main_usart;

// configuration
void init_USART(USART_t *target, int USART_NUM, enum GPIO_pin_number LED_Pin)
{
    // USART setting
/////////////////////////////////////////////
    RCC->APB2ENR |= 0x00004004;
    GPIOA->CRH &= ~(0xFFu << 4);
    GPIOA->CRH |= (0x04B << 4);

    USART1->BRR = BAUDRATE_REG;
    USART1->CR1 = 0x00000000;
    USART1->CR2 = 0x00000000;
    USART1->CR3 = 0x00000000;
    USART1->CR1 |= 0x00000008;      // TE
    USART1->CR1 |= 0x00000004;      // RE
 //   USART1->CR1 |= 0x00000080;      // TXEIE
    USART1->CR1 |= 0x00000020;      // RXEIE

    USART1->CR1 |= 0x00002000;      //UE

    NVIC->ISER[1] |= (1 << 5);
//////////////////////////////////////////////



    target->USART_BASE_ADDR = USART1;

    // LED setting
    init_GPIO_pin(&target->connected_LED, PushPullOut, LED_Pin, 1);

    // members initialize
    target->is_connected = 0;
    target->connection_oppertunity = CONNECTION_MISS_OPPER;
    target->is_sending = 0;

    target->rand_num_Tx = 0;
    target->rand_num_Rx = 1;


    target->recent_Rxstart_time = 0;
    target->Rx_bit_idx = 0;
    target->preamble_cnt = 0;

}


// monitoring
void set_up_monitoring_master(USART_t *target)
{
    request_function_call(monitoring_periodically, CONNECTION_CHK_TIM);
}

void monitoring_periodically()
{
    if(main_usart.rand_num_Tx == main_usart.rand_num_Rx)
    {
        write_val_to_GPIO(&main_usart.connected_LED, 1);
        main_usart.is_connected = 1;
        main_usart.connection_oppertunity = CONNECTION_MISS_OPPER;
    }
    else
    {
        main_usart.connection_oppertunity--;
    }


    if(main_usart.connection_oppertunity <= 0)
    {
        main_usart.connection_oppertunity = 0;
        write_val_to_GPIO(&main_usart.connected_LED, 0);
        main_usart.is_connected = 0;
    }

    monitoring_send();

}
void monitoring_send()     // send a random number
{
    main_usart.rand_num_Tx = rand();
    send_message(&main_usart, Monitoring_repeat, main_usart.rand_num_Tx);
}

void monitoring_rcv(int repeated_num)      // receive repeated random number and managing "IS_CONNECTED" variable
{
    main_usart.rand_num_Rx = repeated_num;
}
void monitoring_repeat_send(int repeated_num)     // repeat a random number
{
    send_message(&main_usart, Monitoring_TX, repeated_num);
}




// transport
int send_message(USART_t *target, enum applicaion_type application_port, int value)        // called by application
{
    if(target->is_sending == 1)
    {
        if(target->sending_application < application_port) // priority check
            return 0;
    }
    // preventing race condition
    // there is no nested interrupt. so, skip semaphore implementation

    if(main_usart.is_connected == 0)
    {
        if(application_port != Monitoring_repeat) // if connection check packet is sending, it can sended
            if(application_port != Monitoring_TX) // if connection check packet is sending, it can sended
                return 0;
    }
    // USART disconnected

    target->is_sending = 1;
    target->sending_application = application_port; 

    //////////


    USART_dataframe_t data_frame_to_send;

    data_frame_to_send.preamble = 0xFFFFFFFF;
    data_frame_to_send.application_port = application_port;
    data_frame_to_send.payload = value;
    data_frame_to_send.EOL = '\0';

    char *frame_cnvrt = (char*)&data_frame_to_send;

    int sending_seq_idx = 0;
    // do
    // {
    //     while (!(target->USART_BASE_ADDR->SR & 0x080));
    //     target->USART_BASE_ADDR->DR = frame_cnvrt[sending_seq_idx];
    //     sending_seq_idx++;
    // } while (frame_cnvrt[sending_seq_idx] != '\0' );


    for(sending_seq_idx = 0 ; sending_seq_idx < sizeof(USART_dataframe_t); sending_seq_idx++)
    {
        while (!(target->USART_BASE_ADDR->SR & 0x080));
        target->USART_BASE_ADDR->DR = frame_cnvrt[sending_seq_idx];
				for(int i = 0; i < 10000; i++); // flow control (prevent nested interrupt
				//sleep(100);
    }
    
    target->is_sending = 0;

    return 1;
}



void demux_message(enum applicaion_type application_port, int value)    // called by ISR
{
    switch(application_port)
    {   
    case Monitoring_TX:
        monitoring_rcv(value);
    break;
    
    case Monitoring_repeat:
        monitoring_repeat_send(value);
    break;
  
    case Command:

    break;
  
    case Temp:
    break;

    default:
        break;
    }
}


void USART1_IRQHandler(void)
{
    if (USART1->SR & 0x20); //is interrupt invoked by receiving
    {
        if(main_usart.is_listening)
        {
            //if(RX_TIME_OUT < now_time() - main_usart.recent_Rxstart_time)
            // if(1000 < now_time() - main_usart.recent_Rxstart_time)
            // {
            //     main_usart.Rx_bit_idx = 0;
            //     main_usart.recent_Rxstart_time = now_time();
            // }   
            // previous packet has been time out.
            // drop previous packet
        }
        else
        {
            main_usart.Rx_bit_idx = 0;
            main_usart.recent_Rxstart_time = now_time();
            main_usart.is_listening = 1;
        }
        // start new receiving

        int RcvData = main_usart.USART_BASE_ADDR->DR;

        //////pre-amble controller/////

        if(RcvData == 0xFF)
        {
            if( main_usart.preamble_cnt == 4 ) // when too long preamble
            {   
                main_usart.Rx_bit_idx--;
                main_usart.preamble_cnt--;
            }
            if( main_usart.preamble_cnt == 3 ) // whenever preamble received
            {   
                main_usart.Rx_bit_idx = main_usart.preamble_cnt;
                main_usart.recent_Rxstart_time = now_time();
            }
            main_usart.preamble_cnt++;
        }
        else if(main_usart.preamble_cnt == 4)
        {
            main_usart.preamble_cnt = 0;
        }
        //////////////////////////////
 
        main_usart.rx_buf[main_usart.Rx_bit_idx] = RcvData;


        if(main_usart.Rx_bit_idx == sizeof(USART_dataframe_t) - 1) // all data frame is received
        {
            main_usart.is_listening = 0;
            main_usart.preamble_cnt = 0;

            USART_dataframe_t *received_message = (USART_dataframe_t*)main_usart.rx_buf;

            demux_message(received_message->application_port, received_message->payload);
            // received.
            // send data to T.L.
        }
        else    // still receiving
        {
            main_usart.Rx_bit_idx++;
        }

    }
}