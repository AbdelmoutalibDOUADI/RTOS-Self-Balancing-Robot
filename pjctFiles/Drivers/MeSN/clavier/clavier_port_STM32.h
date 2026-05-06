/*
 * clavier_port-STM32.h
 *
 *  Created on: 28 may 2018
 *      Author:
 */
#ifndef CLAVIER_PORT_STM32_H_
#define CLAVIER_PORT_STM32_H_

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "stm32l1xx.h"

//-----------------------------------------------------------------------------
// NOP assembly instruction - used for a very short delay
//-----------------------------------------------------------------------------
#define NOP()			__NOP()

//-----------------------------------------------------------------------------
// Definition des I/O
//-----------------------------------------------------------------------------
//  in				 CLAVIER_X1  CLAVIER_X2  CLAVIER_X3
// 				     ----------------------------------
//  out	 CLAVIER_Y1  |	1		    2		    3     |
//	out	 CLAVIER_Y2  |	4		    5		    6     |
// 	out	 CLAVIER_Y3  |	7		    8		    9     |
// 	out	 CLAVIER_Y4  |	*		    0		    #     |
//		         ----------------------------------
// CLAVIER_X1		PA.6
// CLAVIER_X2		PA.7
// CLAVIER_X3		PA.8
// CLAVIER_Y1		PA.9
// CLAVIER_Y2		PA.10
// CLAVIER_Y3		PA.11
// CLAVIER_Y4		PA.12
//-----------------------------------------------------------------------------
#define CLAVIER_Y1		((GPIOA->IDR)&(0x01<<9))
#define CLAVIER_Y2		((GPIOA->IDR)&(0x01<<10))
#define CLAVIER_Y3		((GPIOA->IDR)&(0x01<<11))
#define CLAVIER_Y4		((GPIOA->IDR)&(0x01<<12))

//-----------------------------------------------------------------------------
// Allumer le peripherique GPIOC
//-----------------------------------------------------------------------------
#define	GPIO_ON			RCC->AHBENR|=RCC_AHBENR_GPIOAEN

//-----------------------------------------------------------------------------
// Configurer X en OUTPUT
//-----------------------------------------------------------------------------
#define	CLAVIER_X1_OUTPUT			GPIOA->MODER|=0x01<<(6*2)
#define	CLAVIER_X2_OUTPUT			GPIOA->MODER|=0x01<<(7*2)
#define	CLAVIER_X3_OUTPUT			GPIOA->MODER|=0x01<<(8*2)

//-----------------------------------------------------------------------------
// Configurer Y en INTPUT Pull-Up
//-----------------------------------------------------------------------------
#define	CLAVIER_Y1_INPUT_PU		GPIOA->MODER&=~(0x3<<(9*2));\
															GPIOA->PUPDR|=0x01<<(9*2)
#define	CLAVIER_Y2_INPUT_PU		GPIOA->MODER&=~(0x3<<(10*2));\
															GPIOA->PUPDR|=0x01<<(10*2)
#define	CLAVIER_Y3_INPUT_PU		GPIOA->MODER&=~(0x3<<(11*2));\
															GPIOA->PUPDR|=0x01<<(11*2)
#define	CLAVIER_Y4_INPUT_PU		GPIOA->MODER&=~(0x3<<(12*2));\
															GPIOA->PUPDR|=0x01<<(12*2)

//-----------------------------------------------------------------------------
// Mettre X en PUSH_PULL
//-----------------------------------------------------------------------------
#define	X1_PUSH_PULL			GPIOA->OTYPER&=~(0x01<<6)
#define	X2_PUSH_PULL			GPIOA->OTYPER&=~(0x01<<7)
#define	X3_PUSH_PULL			GPIOA->OTYPER&=~(0x01<<8)

//-----------------------------------------------------------------------------
// Mettre X en OPEN DRAIN
//-----------------------------------------------------------------------------
#define	X1_OPEN_DRAIN			GPIOA->OTYPER|=(0x01<<6)
#define	X2_OPEN_DRAIN			GPIOA->OTYPER|=(0x01<<7)
#define	X3_OPEN_DRAIN			GPIOA->OTYPER|=(0x01<<8)

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Mettre X a 0
//-----------------------------------------------------------------------------
#define CLAVIER_X1_BAS		GPIOA->ODR&=~(0x01<<6)
#define CLAVIER_X2_BAS		GPIOA->ODR&=~(0x01<<7)
#define CLAVIER_X3_BAS		GPIOA->ODR&=~(0x01<<8)

//-----------------------------------------------------------------------------
// Mettre X a 1
//-----------------------------------------------------------------------------
#define CLAVIER_X1_HAUT		GPIOA->ODR|=(0x01<<6)
#define CLAVIER_X2_HAUT		GPIOA->ODR|=(0x01<<7)
#define CLAVIER_X3_HAUT		GPIOA->ODR|=(0x01<<8)

//-----------------------------------------------------------------------------

#endif /* CLAVIER_PORT_STM32_H_ */
