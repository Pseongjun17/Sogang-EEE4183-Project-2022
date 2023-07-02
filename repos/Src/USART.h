// 
//                      USART HEADER FILE
// 
// Author : Park SeongJun
//
//
//
//  This USART Library's target is ARM CORTEX M3 STM103
//  which includes controlls USART1
//  
//  
//  o Specification
//      Support Full-duplex communication
//      Periodically connection test
//      support 4 applications
//      
//      
//      
//  o Structure
//      
//          ┌----------------------┐
//          |                      |
//          |      Application     |
//          |                      |
//          └----------------------┘     // monitoring, clock... etc
//                 |        ^
//                 v        |   
//                  
//          ┌----------------------┐
//          |                      |
//          |   message transport  |     // capsulate data into data frame && MUX TX/RX Channel
//          |                      |     // Queue the given Tx message
//          └----------------------┘     // "send_message", "demux_message"
//                 |        ^
//                 v        |
//                  
//          ┌----------------------┐
//          |                      |     // Tx : flush Tx Queue
//          |     USART TX, RX     |     // Rx : Receive byte-stream & construct data frame from given bytes
//          |                      |     // push each bytes into TDR, RDR
//          └----------------------┘     // Tx_Link(Tx), USART1_IRQHandler(Rx) 
//                 |        ^            
//                 v        |
//                  
//          ┌----------------------┐
//          |                      |
//          |    Physical cable    |
//          |                      |     // Jump Cable
//          └----------------------┘
//          
//          
//          
//  o data frame
//      ┌-----------------------┐
//      |       Pre-amble       |
//      +-----------------------+ 
//      | application identifier|
//      +-----------------------+
//      |        Payload        |
//      +-----------------------+
//      |    End-of-Line sign   |
//      └-----------------------┘
//  -> This struct type is converted to character pointer by pointer type-casting
//
//      
//      
//      
//  o Etc
//      it's USART Tx (send_message) handles race conditions
//      it's USART Rx (USART1_IRQHandler) Drops time-out packet. So, set the time-out value carefully
//      Data Frame is scalable. you can add any data field into "struct USART_dataframe_st"
//          because it's data frame has End Of Line sign
//      
//      Eventhough it handles race condition, it can not blocks any Tx sequence. 
//      When conflict occures, this system just drops lower priority packet.
//      So, use priority. USER message should be higher priority
//      Because system message which is transmited periodically can be recovered automatically even when droped.
//          **AND NEVER USE LOOP SENDING WHILE SEND_MESSAGE==0 -> It may blocks all system
// 
// 


#ifndef _USART_H_
#define _USART_H_
 
#include <stm32f10x.h>
#include <stdbool.h>
#include <stdlib.h>
#include "GPIO.h"
#include "TIMER.h"
#include "DOT_MATRIX.h"
#include "GlobalParameters.h"






enum applicaion_type
{
    Command,
    Temp_Send,
    Monitoring_TX,
    Monitoring_repeat
};      // of receiver's


typedef struct USART_dataframe_st
{
    unsigned int preamble;
    enum applicaion_type application_port;
    int payload;
    bool is_minus;
    char EOL
}USART_dataframe_t;


typedef struct USART_moudle_st
{
    USART_TypeDef *USART_BASE_ADDR;
    bool is_connected;
    int connection_oppertunity;

    bool is_sending;
    bool is_queueing;
    enum applicaion_type queueing_application;
    bool is_listening;
    
    int rand_num_Tx;
    int rand_num_Rx;
    GPIO_PORT_t connected_LED;

    USART_dataframe_t Tx_Queue[Tx_QUEUE_SIZE];
    int queue_head;
    int queue_rear;
    
    int Rx_bit_idx;
    int preamble_cnt;
    char rx_buf[20]; 

    int recent_Rxstart_time;   
    // if USART disconnected while RX is processing,
    // this counter catch timeout using TIM1 Val
}USART_t;

extern USART_t main_usart;


// configuration
void init_USART(USART_t *target, int USART_NUM, enum GPIO_pin_number LED_Pin);


// monitoring
void set_up_monitoring_master(USART_t *target);

void monitoring_periodically();
void monitoring_send();     // send a random number
void monitoring_rcv(int repeated_num);      // receive repeated random number and managing "IS_CONNECTED" variable
void monitoring_repeat_send(int repeated_num); // repeat a random number


// transport
int send_message(USART_t *target, enum applicaion_type, int value);        // called by application
void demux_message(enum applicaion_type, int value);    // called by ISR


// link
void Tx_Link(void);

#endif


