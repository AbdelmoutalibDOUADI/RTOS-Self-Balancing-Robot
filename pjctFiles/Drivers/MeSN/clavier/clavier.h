/*
 * clavier.h
 *
 *  Created on: 11 avr. 2014
 *      Author:
 */
#ifndef CLAVIER_H_
#define CLAVIER_H_

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#if defined (SI_C8051F930)
	#include <compiler_defs.h>
	#include <SI_C8051F930_Register_Enums.h>	// SFR declarations for SiLabs target
#endif

//-----------------------------------------------------------------------------
// Prototypes de fonctions
//-----------------------------------------------------------------------------
uint8_t get_touche(void);
#if defined (STM32L152xE)
	void clavier_init(void);
#endif

//-----------------------------------------------------------------------------
#endif /* CLAVIER_H_ */
