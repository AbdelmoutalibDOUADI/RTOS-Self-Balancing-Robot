//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#if defined (SI_C8051F930)
	#include <SI_C8051F930_Register_Enums.h>                // SFR declarations
	#include "LCD_port.h"
#elif defined (STM32L152xE)
	#include "LCD_port_STM32.h"	// Low level layer for STM32 target
#endif

#include "LCD.h"


//-----------------------------------------------------------------------------
// init_LCD() :
//
// Initialisation de l'afficheur en mode 4 bits
//-----------------------------------------------------------------------------
void init_LCD(void){
    unsigned char u;

  AFFICHEUR_ON;
  AFFICHEUR_RS_BAS;

  for(u=0;u<250;u++) delay(200);									// attente 50ms afficheur ON

	AFFICHEUR_DB=0X30|(AFFICHEUR_DB & AFFICHEUR_DB_MASK); PULSE_EN;
	for(u=0;u<42;u++) delay(100); 									// attente 4.2ms
	AFFICHEUR_DB=0X30|(AFFICHEUR_DB & AFFICHEUR_DB_MASK); PULSE_EN; 	delay(100);	// attente 0.1ms
	AFFICHEUR_DB=0X30|(AFFICHEUR_DB & AFFICHEUR_DB_MASK); PULSE_EN; 	delay(40);
	AFFICHEUR_DB=0X20|(AFFICHEUR_DB & AFFICHEUR_DB_MASK); PULSE_EN; 	delay(40);
	
	AFFICHEUR_DB=0X00|(AFFICHEUR_DB & AFFICHEUR_DB_MASK); PULSE_EN;	delay(40);
	AFFICHEUR_DB=0XE0|(AFFICHEUR_DB & AFFICHEUR_DB_MASK); PULSE_EN;	delay(40);

	AFFICHEUR_DB=0X00|(AFFICHEUR_DB & AFFICHEUR_DB_MASK); PULSE_EN;	delay(40);
	AFFICHEUR_DB=0X60|(AFFICHEUR_DB & AFFICHEUR_DB_MASK); PULSE_EN;	delay(40);

	AFFICHEUR_RS_HAUT;
}

//-----------------------------------------------------------------------------
// sendByte_LCD() :
//-----------------------------------------------------------------------------
static void sendByte_LCD(unsigned char u){
	AFFICHEUR_DB= (u & 0xF0)|(AFFICHEUR_DB & AFFICHEUR_DB_MASK);	PULSE_EN;
	AFFICHEUR_DB= (u<<4)|(AFFICHEUR_DB & AFFICHEUR_DB_MASK);	PULSE_EN; delay(40);
}

//-----------------------------------------------------------------------------
// sendByte_LCD() :
//-----------------------------------------------------------------------------
void clrscr_LCD(void){
	unsigned char u;

	AFFICHEUR_RS_BAS;
	sendByte_LCD(0x01);
	for(u=0;u<17;u++) delay(100); 			//1.7ms (min 1.64ms)
	AFFICHEUR_RS_HAUT;
}

//-----------------------------------------------------------------------------
// putc_LCD() :
//-----------------------------------------------------------------------------
char putC_LCD(char v){
	if(v=='\r')			{AFFICHEUR_RS_BAS;sendByte_LCD(0x80);}
	else if(v=='\n')	{AFFICHEUR_RS_BAS;sendByte_LCD(0xC0);}
	else				{AFFICHEUR_RS_HAUT;sendByte_LCD(v);}
	AFFICHEUR_RS_HAUT;
	return(0);
}

//-----------------------------------------------------------------------------
// deinit_LCD() :
//-----------------------------------------------------------------------------
void deinit_LCD(void){
	AFFICHEUR_OFF;
}
