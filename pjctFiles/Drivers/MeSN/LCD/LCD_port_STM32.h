/*
 * LCD_port_STM32.h
 *
 *  Created on: 11 avr. 2014
 *      Author:
 */
#ifndef LCD_PORT_STM32_H_
#define LCD_PORT_STM32_H_

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "stm32l1xx.h"		// SFR declarations
/*TODO : a completer*/
#include "timeBase.h"		//provide delay routine

//-----------------------------------------------------------------------------
// Delay routine
//-----------------------------------------------------------------------------
// MeSN_Delay() propose une attente bloquante en multiple de 100µs
// delay() passe un paramètre exprimé en µs -> facteur de conversion 1/100
#define delay(__temps__)	MESN_Delay((__temps__)/100)

//-----------------------------------------------------------------------------
// NOP assembly instruction - used for a very short delay
//-----------------------------------------------------------------------------
#define NOP()		__NOP()

//-----------------------------------------------------------------------------
// Allumer le peripherique GPIOC
//-----------------------------------------------------------------------------
#define	GPIO_ON			RCC->AHBENR|=RCC_AHBENR_GPIOCEN

//-----------------------------------------------------------------------------
// Definitions pour l'afficheur
//
// AFFICHEUR_DB : utilisation de 4 broches du port C (PC.4/5/6/7)
// AFFICHEUR_EN : PC.0
// AFFICHEUR_RS : PC.1
// AFFICHEUR_POWER : PC.2
//-----------------------------------------------------------------------------
#define	AFFICHEUR_DB				GPIOC->ODR
#define AFFICHEUR_DB_MASK			0xFF0F

#define AFFICHEUR_EN_BAS			GPIOC->ODR&=~(0x01<<0)
/*TODO : a completer*/
#define AFFICHEUR_RS_BAS			GPIOC->ODR&=~(0x01<<1)
#define AFFICHEUR_POWER_BAS			GPIOC->ODR&=~(0x01<<2)


/*TODO : a completer*/
#define AFFICHEUR_EN_HAUT			GPIOC->ODR|=(0x01<<0)
#define AFFICHEUR_RS_HAUT			GPIOC->ODR|=(0x01<<1)
#define AFFICHEUR_POWER_HAUT		GPIOC->ODR|=(0x01<<2)


//-----------------------------------------------------------------------------
// AFFICHEUR ON :
//
// 1. Activation du module GPIO (GPIOC)
// 2. les quatre bits de poids fort de AFFICHEUR_DB sont mis a '1'
// 3. AFFICHEUR_RS=1, AFFICHEUR_EN=0, AFFICHEUR_POWER=1
// 4. AFFICHEUR_DB, AFFICHEUR_RS, AFFICHEUR_EN et AFFICHEUR_POWER configures
//    en output push-pull
//-----------------------------------------------------------------------------
#define	AFFICHEUR_ON \
		GPIO_ON;\
		AFFICHEUR_DB|=0xF0;\
		AFFICHEUR_RS_HAUT;\
		AFFICHEUR_EN_BAS;\
		AFFICHEUR_POWER_HAUT;\
		GPIOC->MODER|=((0x1<<0*2)|(0x1<<1*2)|(0x1<<2*2)|0x00005500);\
		GPIOC->OTYPER&=~((0x1<<0)|(0x1<<1)|(0x1<<2)|0xF0)

//-----------------------------------------------------------------------------
// AFFICHEUR OFF :
//
// 1. les quatre bits de poids fort de AFFICHEUR_DB sont mis a '1'
// 2. AFFICHEUR_RS=1, AFFICHEUR_EN=1, AFFICHEUR_POWER=0
// 3. AFFICHEUR_DB, AFFICHEUR_RS, AFFICHEUR_EN et AFFICHEUR_POWER configures
//    en input (reset state)
//-----------------------------------------------------------------------------
/*TODO : a completer*/
#define	AFFICHEUR_OFF \
		AFFICHEUR_DB|=0xF0;\
		AFFICHEUR_RS_HAUT;\
		AFFICHEUR_EN_HAUT;\
		AFFICHEUR_POWER_BAS;\
		GPIOC->MODER &=~((0x3<<0*2)|(0x3<<1*2)|(0x3<<2*2)|0x0000FF00);


//-----------------------------------------------------------------------------
// PULSE_EN :
//
// La duree du pulse depend de l'afficheur.
//-----------------------------------------------------------------------------
#define PULSE_EN	AFFICHEUR_EN_HAUT;\
									NOP();\
									AFFICHEUR_EN_BAS

//-----------------------------------------------------------------------------
#endif /* LCD_PORT_STM32_H_ */
