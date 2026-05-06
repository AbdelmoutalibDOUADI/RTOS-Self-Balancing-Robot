/*
 * timeBase.c
 *
 *  Created on: 2 juin 2018
 *      Author: bdufay
 */

#include "timeBase.h"

/* Private definition --------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
volatile uint32_t timeSinceLastReset=0;

/* Private prototype----------------------------------------------------------*/
void              IncTime(void);
uint32_t          MESN_GetTime(void);

/* Public Functions-----------------------------------------------------------*/
/**
  * @brief This function configure TIM6 timer to generate interrupts at
  *        regular time intervals where "timeSinceLastReset" is incremented.
  * @note In the default implementation , source CLOCK is .......
  *       and the autoreload value is .......
  *       As a consequence, interrupt time interval is ........
  * @note This function must be called AFTER setting the system clock
  * @retval None
  */
void MESN_InitTimeBase(void){

	uint32_t reloadVal = 1;
	uint32_t prescalVal = 0;

	/* Set the prescaler value according to system clock */
	if(SystemCoreClock >= 10000000){
		prescalVal = 9;
	}else{
		prescalVal = 0;
	}
	/* Evaluate the Autorelaod value based on system clock */
	reloadVal = (SystemCoreClock/(prescalVal+1)/TIMEBASE_FREQ);

	/* Peripheral clock enable */
	RCC->APB1ENR|=RCC_APB1ENR_TIM6EN;

	/* Set the Prescaler value */
	*((volatile uint32_t *)(0x40001028)) = (uint16_t)prescalVal;	//PSC register
	/* Set the Autoreload value */
	*((volatile uint32_t *)(0x4000102C)) = (uint16_t)reloadVal;		//ARR register
	/* Force an update event to reload the Prescaler register */
	TIM6->EGR = 0x0001;
	/* Allow interrupt request generation */
	TIM6->DIER |= 0x0001;
	/* Reset counter value*/
	TIM6->CNT = 0;
	/* Enable counter and update event */
	TIM6->CR1 = 0x0001;

	/* Enable interrupt */
	NVIC_EnableIRQ(TIM6_IRQn);
}

/**
  * @brief This function provides accurate delay based
  *        on variable incremented.
  * @note In the default implementation , TIM6 timer is the source of time base.
  *       It is used to generate interrupts at regular time intervals where
  *       timeFromStart is incremented.
  * @param Delay: specifies the delay time length, in ..........
  * @retval None
  */
void MESN_Delay(volatile uint32_t waitingTime){
  uint32_t startTime = 0;

  startTime = MESN_GetTime();

  /* wait at least 1 tick even if waitingTime parameter is < 1 */
  if(waitingTime==0){
  	waitingTime = 1;
  }

  while((MESN_GetTime() - startTime) < waitingTime)
  {
  }
}

/*Private functions-----------------------------------------------------------*/
/**
* @brief This function handles TIM6 timer IRQ.
*/
void TIM6_IRQHandler(void){

	/* Clear the interrupt pending bits */
	TIM6->SR = ~0x0001;

	/* Process ISR */
	IncTime();
}

/**
  * @brief This function is called to increment  a global variable
  *        "timeFromStart" used as application time base.
  * @note In the default implementation, this variable is incremented
  *       in TIM6 ISR.
  * @retval None
  */
void IncTime(void)
{
	timeSinceLastReset++;
}

/**
  * @brief Provides a time value.
  * @retval time value
  */
uint32_t MESN_GetTime(void)
{
  return timeSinceLastReset;
}


