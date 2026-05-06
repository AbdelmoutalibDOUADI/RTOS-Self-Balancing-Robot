/*
 * LCD.h
 *
 *  Created on: 11 avr. 2014
 *      Author:
 */
#ifndef LCD_H_
#define LCD_H_

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#if defined (SI_C8051F930)
	#include <compiler_defs.h>
	#include <SI_C8051F930_Register_Enums.h>                // SFR declarations
#endif

//-----------------------------------------------------------------------------
// Prototype de fonction
//-----------------------------------------------------------------------------
void init_LCD(void);
char putC_LCD(char);
void clrscr_LCD(void);
void deinit_LCD(void);

//-----------------------------------------------------------------------------
#endif /* AFFICHEUR_H_ */
