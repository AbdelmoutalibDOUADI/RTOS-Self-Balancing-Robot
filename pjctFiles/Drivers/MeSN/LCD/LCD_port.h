/*
 * afficheur.h
 *
 *  Created on: 11 avr. 2014
 *      Author:
 */
#ifndef LCD_PORT_H_
#define LCD_PORT_H_

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <SI_C8051F930_Register_Enums.h>                // SFR declarations
#include "time.h"

//-----------------------------------------------------------------------------
// D�finitions pour l'afficheur
//
// AFFICHEUR_DB : utilisation uniquement du poids fort de P1 (P1.4/5/6/7)
//-----------------------------------------------------------------------------
#define	AFFICHEUR_DB	P1
#define	AFFICHEUR_DB_MASK	0x0F

SBIT (AFFICHEUR_RS, P1, 3);
SBIT (AFFICHEUR_EN,	P1, 1);
SBIT (AFFICHEUR_POWER, P1, 2);

#define AFFICHEUR_RS_BAS			AFFICHEUR_RS=0
#define AFFICHEUR_EN_BAS			AFFICHEUR_EN=0
#define AFFICHEUR_POWER_BAS		AFFICHEUR_POWER=0

#define AFFICHEUR_RS_HAUT			AFFICHEUR_RS=1
#define AFFICHEUR_EN_HAUT			AFFICHEUR_EN=1
#define AFFICHEUR_POWER_HAUT	AFFICHEUR_POWER=1

//-----------------------------------------------------------------------------
// AFFICHEUR OFF :
//
// => les quatre bits de poids fort de AFFICHEUR_DB sont mis � '1'
// => AFFICHEUR_RS=1, AFFICHEUR_EN=1, AFFICHEUR_POWER=0
// => AFFICHEUR_DB, AFFICHEUR_RS, AFFICHEUR_EN et AFFICHEUR_POWER en open-drain
//-----------------------------------------------------------------------------
#define	AFFICHEUR_OFF		P1|=0xF0;\
												AFFICHEUR_RS_HAUT;\
												AFFICHEUR_EN_HAUT;\
												AFFICHEUR_POWER_BAS;\
												P1MDOUT&=~0xF0;\
												P1MDOUT&=~0x0E

//-----------------------------------------------------------------------------
// AFFICHEUR ON :
//
// => les quatre bits de poids fort de AFFICHEUR_DB sont mis � '1'
// => AFFICHEUR_RS=1, AFFICHEUR_EN=0, AFFICHEUR_POWER=1
// => AFFICHEUR_DB, AFFICHEUR_RS, AFFICHEUR_EN et AFFICHEUR_POWER en push-pull
//-----------------------------------------------------------------------------
#define	AFFICHEUR_ON	P1|=0xF0;\
											AFFICHEUR_RS_HAUT;\
											AFFICHEUR_EN_BAS;\
											AFFICHEUR_POWER_HAUT;\
											P1MDOUT|=0xF0;\
											P1MDOUT|=0x0E

//-----------------------------------------------------------------------------
// PULSE_EN :
//
// La dur�e du pulse depend de l'afficheur.
//-----------------------------------------------------------------------------
#define PULSE_EN	AFFICHEUR_EN_HAUT;\
									NOP();\
									AFFICHEUR_EN_BAS

//-----------------------------------------------------------------------------
#endif /* AFFICHEUR_H_ */
