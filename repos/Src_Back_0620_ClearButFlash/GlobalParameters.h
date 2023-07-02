//      2022 MicroProcessor Term-Project
//            - implement clock -
//
//  Author : 20171533 Park SeongJun
//           201715** Choi SeongU
//
//
//
//

#ifndef _GLOBALPARAMETERS_H_
#define _GLOBALPARAMETERS_H_

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

/////////   TARGET SETTING   /////////
//#define TX_BOARD
#define RX_BOARD




///////// Global Varibles ////////////
extern int32_t RecentCommand;
extern bool CommandRecieved;
extern int TempVal;

#define ONE_SEC     (40000)

/////////   USART SETTING   /////////
#ifdef RX_BOARD
#define CONNECTION_CHK_TIM      (1500)
#endif
#ifdef TX_BOARD
#define CONNECTION_CHK_TIM      (1700)
#endif

// it is also used for time out chking
#define RX_TIME_OUT             (4)                                                             
// UNIT : second
#define CONNECTION_MISS_OPPER   (4)
#define Tx_QUEUE_SIZE           (20)
#define BAUDRATE_REG            (0xAA6)


//////// Key Matrix Setting  //////// 
#define KEY_SCAN_INTERVAL   (52)
#define KEY_MAT_ROW_SIZE   (4)
#define KEY_MAT_COL_SIZE   (4)

#define INVALID_KEY_MAT_INPUT (-1)



//////// Dot Matrix Setting  //////// 
#define ROW_SCAN_INTERVAL   (2)
#define DOT_MAT_ROW_SIZE   (8)
#define DOT_MAT_COL_SIZE   (16)
#define DOT_MAT_CHAR_SIZE   (8)

#define FLASH_INTERVAL     (1500)
#define ROLLING_INTERVAL  (800)



#define CALI_VAL  (10)




#endif