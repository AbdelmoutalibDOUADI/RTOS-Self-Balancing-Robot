
//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#if defined (SI_C8051F930)
	#include "clavier_port.h"				// Low level layer for SiLabs target
#elif defined (STM32L152xE)
	#include "clavier_port_STM32.h"	// Low level layer for STM32 target
#endif

#include "clavier.h"

//------------------------------------------------------------------------------
// Activation:     Mettre X en PUSH_PULL  puis a 0
//------------------------------------------------------------------------------
#define	ACTIVATION_X1	  X1_PUSH_PULL; CLAVIER_X1_BAS
#define	ACTIVATION_X2	  X2_PUSH_PULL; CLAVIER_X2_BAS
#define	ACTIVATION_X3	  X3_PUSH_PULL; CLAVIER_X3_BAS

//------------------------------------------------------------------------------
// Desactivation   Mettre X a 1 puis en OPEN DRAIN
//------------------------------------------------------------------------------
#define	DESACTIVATION_X1 CLAVIER_X1_HAUT; X1_OPEN_DRAIN
#define	DESACTIVATION_X2 CLAVIER_X2_HAUT; X2_OPEN_DRAIN
#define	DESACTIVATION_X3 CLAVIER_X3_HAUT; X3_OPEN_DRAIN

//------------------------------------------------------------------------------
// Attente pour un changement de niveau :  1 CLK
//------------------------------------------------------------------------------
#define	ATTENTE_NIVEAU_STABLE  NOP()

//------------------------------------------------------------------------------
// get_touche() Routine
//
// Parametre de sortie :
// 	= 0 			: si aucune touche appuyee
// 	= 0X7F			: si plusieurs touches appuyees
// 	= code ASCII	: de l'unique touche appuyee
//
// -----------------------------------------------------------------------------
uint8_t get_touche(void)
{
	unsigned char code_ASCII;
	unsigned char nb_touche=0;

	ACTIVATION_X1;
	ATTENTE_NIVEAU_STABLE;
	if(CLAVIER_Y1==0) {code_ASCII='1';nb_touche++;}
	if(CLAVIER_Y2==0) {code_ASCII='4';nb_touche++;}
	if(CLAVIER_Y3==0) {code_ASCII='7';nb_touche++;}
	if(CLAVIER_Y4==0) {code_ASCII='*';nb_touche++;}
	DESACTIVATION_X1;
	ACTIVATION_X2;
	ATTENTE_NIVEAU_STABLE;
	if(CLAVIER_Y1==0) {code_ASCII='2';nb_touche++;}
	if(CLAVIER_Y2==0) {code_ASCII='5';nb_touche++;}
	if(CLAVIER_Y3==0) {code_ASCII='8';nb_touche++;}
	if(CLAVIER_Y4==0) {code_ASCII='0';nb_touche++;}
	DESACTIVATION_X2;
	ACTIVATION_X3;
	ATTENTE_NIVEAU_STABLE;
	if(CLAVIER_Y1==0) {code_ASCII='3';nb_touche++;}
	if(CLAVIER_Y2==0) {code_ASCII='6';nb_touche++;}
	if(CLAVIER_Y3==0) {code_ASCII='9';nb_touche++;}
	if(CLAVIER_Y4==0) {code_ASCII='#';nb_touche++;}
	DESACTIVATION_X3;

	if(nb_touche==0)		return(0);
	else if(nb_touche!=1)	return(0X7F);
	else 					return(code_ASCII);
}

#if defined (STM32L152xE)
	//----------------------------------------------------------------------------
	// clavier_init() Routine
	// ---------------------------------------------------------------------------
	void clavier_init(void)
	{
		GPIO_ON;

		CLAVIER_X1_OUTPUT;
		CLAVIER_X2_OUTPUT;
		CLAVIER_X3_OUTPUT;

		CLAVIER_Y1_INPUT_PU;
		CLAVIER_Y2_INPUT_PU;
		CLAVIER_Y3_INPUT_PU;
		CLAVIER_Y4_INPUT_PU;
	}
#endif
