/*
 * timeBase.h
 *
 *  Created on: 2 juin 2018
 *      Author: bdufay
 */

#ifndef MESN_TIMEBASE_H_
#define MESN_TIMEBASE_H_

#include "stm32l1xx.h"		//SFR definition

/* Public definitions --------------------------------------------------------*/
#define TIMEBASE_FREQ	10000		// desired TIM6 overflow frequency (in Hz)

/* Public function prototypes ------------------------------------------------*/
void MESN_InitTimeBase (void);
void MESN_Delay(uint32_t waitingTime);


#endif /* MESN_TIMEBASE_H_ */
