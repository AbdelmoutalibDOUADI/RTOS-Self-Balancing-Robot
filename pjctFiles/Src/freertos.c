/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "MotorDriver.h"					// Pilote du moteur
#include "lsm6ds3.h"						// Pilote de la centrale inertielle
#include "mesn_uart.h"						// Pilote de la liaison UART
#include "libSBR_autom_obs-corr.h"			// Algo observateur et correcteur
#include "LCD.h"
#include "clavier.h"
#include "timeBase.h"
#define BUFFER_SIZE 100
#define SEUIL_ANGLE 25000         // 25 degrés en milli-degrés
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
	volatile int32_t buffer[BUFFER_SIZE];
	volatile uint32_t indexW;
	volatile uint32_t indexR;
	volatile uint32_t eltNb;
} bufferCirculareTypeDef;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
bufferCirculareTypeDef circBuff;

osMessageQDef(queueStream, 10, uint32_t);
osMessageQId queueStream;

osMessageQDef(queueAngle, 30, uint32_t);
osMessageQId queueAngle;

osMutexDef(mutexGpio);
osMutexId mutexGpio;

osMutexDef(mutexBuff);
osMutexId mutexBuff;

osMutexDef(mutexUart);
osMutexId mutexUart;

/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void Asservissement(void const *argument);
void Enregistrement(void const *argument);
void Communication(void const *argument);

void writeToBuffer(bufferCirculareTypeDef *buf, int32_t data); //fonction pour ecrire dans Buffer
int32_t readFromBuffer(bufferCirculareTypeDef *buf, uint32_t index); //fonction pour lire  dans Buffer
void afficherAide(void);
void afficherInvite(void);

void afficherAideLCD(void);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const *argument);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName) {
	/* Run time stack overflow checking is performed if
	 configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
	 called if a stack overflow is detected. */
	MESN_UART_PutString_Poll(
			(uint8_t*) "\r\nERROR : stack overflow from task ");
	MESN_UART_PutString_Poll((uint8_t*) pcTaskName);
	while (1)
		;
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
void vApplicationMallocFailedHook(void) {
	/* vApplicationMallocFailedHook() will only be called if
	 configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
	 function that will get called if a call to pvPortMalloc() fails.
	 pvPortMalloc() is called internally by the kernel whenever a task, queue,
	 timer or semaphore is created. It is also called by various parts of the
	 demo application. If heap_1.c or heap_2.c are used, then the size of the
	 heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	 FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	 to query the size of free heap space that remains (although it does not
	 provide information on how the remaining heap might be fragmented). */
	MESN_UART_PutString_Poll((uint8_t*) "\r\nERROR : Heap full!");
	while (1)
		;
}
/* USER CODE END 5 */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void) {
	/* USER CODE BEGIN Init */
	MESN_UART_Init();
	circBuff.eltNb = 0;
	circBuff.indexR = 0;
	circBuff.indexW = 0;
	/* USER CODE END Init */

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	mutexGpio = osMutexCreate(osMutex(mutexGpio));
	mutexBuff = osMutexCreate(osMutex(mutexBuff));
	mutexUart = osMutexCreate(osMutex(mutexUart));
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	queueStream = osMessageCreate(osMessageQ(queueStream), NULL);
	queueAngle = osMessageCreate(osMessageQ(queueAngle), NULL);
	/* USER CODE END RTOS_QUEUES */

	/* Create the thread(s) */
	/* definition and creation of defaultTask */
	//osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 256);
	//defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	osThreadDef(TASK1, Asservissement, osPriorityHigh, 0, 256);
	osThreadCreate(osThread(TASK1), NULL);

	osThreadDef(TASK2, Enregistrement, osPriorityAboveNormal, 0, 256);
	osThreadCreate(osThread(TASK2), NULL);

	osThreadDef(TASK3, Communication, osPriorityNormal, 0, 256);
	osThreadCreate(osThread(TASK3), NULL);
	/* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
//void StartDefaultTask(void const *argument) {}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void Asservissement(void const *argument) {
	uint32_t DernierReveil = 0;
	int32_t acc;
	int32_t vitRot;
	int32_t angleobs;
	int32_t commandeLaw;
	uint8_t LSM6DS3_Res = 0;

	/******* Initialisation dans la tâche de priorité elevee ******/
	MESN_InitTimeBase();
	init_LCD();
	clavier_init();
	MotorDriver_Init();
	MESN_UART_Init();
	if (LSM6DS3_begin(&LSM6DS3_Res) != IMU_SUCCESS) {
	    MESN_UART_PutString_Poll((uint8_t*) "\r\nErreur IMU !");
	    while (1);
	}
	clrscr_LCD();
	// j ai essayee sans osDelay mais dans terminal m'affiche :Erreur IMU !
	//la solution est de fait un os Delay pour  assurer une initialisation correcte du matériel
	osDelay(400);
	/*********************************************/
	DernierReveil = osKernelSysTick();

	for (;;) {
		osDelayUntil(&DernierReveil, 10);

		/* Lecture des capteurs */
		LSM6DS3_readMgAccelX(&acc);
		LSM6DS3_readMdpsGyroY(&vitRot);

		/* Calcul de l'angle entre l'axe du robot et l'axe vertical */
		angleobs = autoAlgo_angleObs(acc, vitRot);

		/* Calcul de la loi de commande */
		commandeLaw = autoAlgo_commandLaw(angleobs, vitRot);

		/* Application de la commande moteur avec protection mutex */
		if (osMutexWait(mutexGpio, 10) == osOK) {
			MotorDriver_Move(commandeLaw);
			osMutexRelease(mutexGpio);
		}

		/* Envoi de l'angle à la tâche d'enregistrement */
		osMessagePut(queueAngle, (uint32_t) angleobs, 2);
	}
}

void Enregistrement(void const *argument) {
	int32_t angle;
	osEvent evt;

	for (;;) {
		/* Attente des données d'angle de la TASK d'asservissement */
		evt = osMessageGet(queueAngle, osWaitForever);

		if (evt.status == osEventMessage) {
			angle = (int32_t) evt.value.v;

			/* Stockage dans le buffer circulaire */
			writeToBuffer(&circBuff, angle);

			/* Envoi vers la queue de streaming */
			osMessagePut(queueStream, (uint32_t) angle, 0);

			/* Indication LED : clignotement si angle > 25 deg */
			if (abs(angle) > SEUIL_ANGLE) {
				if (osMutexWait(mutexGpio, 10) == osOK) {
					HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
					osMutexRelease(mutexGpio);
				}
			}
		}
	}
}


void Communication(void const *argument)
{
    uint8_t cmd[20];
    uint8_t out[80];
    int32_t val;
    uint32_t i;
    osEvent evt;

   // osDelay(300);
    afficherAideLCD();
    afficherAide();
    afficherInvite();

    for (;;)
    {
        /* Attente commande UART */
        if (MESN_UART_GetString(cmd, osWaitForever) == USER_OK)
        {
            /* read */
            if (strcmp((char*)cmd, "read") == 0)
            {
            	//verification est ce que buffer est vide
                if (circBuff.eltNb > 0)
                {

                    val = readFromBuffer(&circBuff, circBuff.eltNb - 1);//recuperation de la derniere valeur
                    sprintf((char*)out, "\r\nLast angle: %ld mdeg", val);
                }
                else
                {
                    sprintf((char*)out, "\r\nNo data");
                }

                osMutexWait(mutexUart, osWaitForever);
                MESN_UART_PutString_Poll(out);
                osMutexRelease(mutexUart);
            }

            /* dump */
            else if (strcmp((char*)cmd, "dump") == 0)
            {
                osMutexWait(mutexUart, osWaitForever);
                MESN_UART_PutString_Poll((uint8_t*)"\r\ndump:");
                osMutexRelease(mutexUart);

                for (i = 0; i < circBuff.eltNb; i++)
                {
                	osMutexWait(mutexUart, osWaitForever);
                    val = readFromBuffer(&circBuff, i);
                    sprintf((char*)out, "\r\n[%lu] %ld mdeg", i, val);
                    MESN_UART_PutString_Poll(out);
                    osMutexRelease(mutexUart);
                }
            }

            /* stream */
            else if (strcmp((char*)cmd, "stream") == 0)
            {
                osMutexWait(mutexUart, osWaitForever);
                MESN_UART_PutString_Poll((uint8_t*)"\r\nStreaming mode - press ENTER to quit\r\n");
                osMutexRelease(mutexUart);

                /* Boucle de streaming */
                while (1)
                {
                    /* Récupérer la valeur depuis la queue */
                    evt = osMessageGet(queueStream, 50);

                    if (evt.status == osEventMessage)
                    {
                        val = (int32_t)evt.value.v;
                        sprintf((char*)out, "\r%ld mdeg     ", val);

                        osMutexWait(mutexUart, osWaitForever);
                        MESN_UART_PutString_Poll(out);
                        osMutexRelease(mutexUart);
                    }

                    /* Vérifier si l'utilisateur a appuyé sur ENTER pour quitter */
                    if (MESN_UART_GetString(cmd, 0) == USER_OK)
                    {
                        break;
                    }
                }
            }

            /* help */
            else if (strcmp((char*)cmd, "help") == 0)
            {
                afficherAide();
            }

            /* unknown */
            else
            {
                osMutexWait(mutexUart, osWaitForever);
                MESN_UART_PutString_Poll((uint8_t*)"\r\nUnknown command");
                osMutexRelease(mutexUart);
            }

            afficherInvite();
        }

        osDelay(10);
    }
}

/* ****** AIDE ****** */

void afficherAide(void)
{
    osMutexWait(mutexUart, osWaitForever);
    MESN_UART_PutString_Poll((uint8_t*)"\r\nCommands:");
    MESN_UART_PutString_Poll((uint8_t*)"\r\n read   - return last measured angle value");
    MESN_UART_PutString_Poll((uint8_t*)"\r\n dump   - return last hundred angle values");
    MESN_UART_PutString_Poll((uint8_t*)"\r\n stream - continuously return last measured angle value and update display");
    MESN_UART_PutString_Poll((uint8_t*)"\r\n          press ENTER to quit stream mode");
    MESN_UART_PutString_Poll((uint8_t*)"\r\n help   - print this menu");
    osMutexRelease(mutexUart);
}
/* **** INVITE *******/
void afficherInvite(void)
{
    osMutexWait(mutexUart, osWaitForever);
    MESN_UART_PutString_Poll((uint8_t*)"\r\nSelfBalancingRobot:~# ");
    osMutexRelease(mutexUart);
}
/* ****** AIDE LCD****** */
void afficherAideLCD(void){

}
/* ********** BUFFER CIRCULAIRE *********** */

void writeToBuffer(bufferCirculareTypeDef *buf, int32_t data)
{
    osMutexWait(mutexBuff, osWaitForever);

    buf->buffer[buf->indexW] = data;
    buf->indexW = (buf->indexW + 1) % BUFFER_SIZE;

    if (buf->eltNb < BUFFER_SIZE)
        buf->eltNb++;

    osMutexRelease(mutexBuff);
}

int32_t readFromBuffer(bufferCirculareTypeDef *buf, uint32_t index)
{
    int32_t data = 0;

    osMutexWait(mutexBuff, osWaitForever);

    if (index < buf->eltNb)
    {
        uint32_t i = (buf->indexR + index) % BUFFER_SIZE;
        data = buf->buffer[i];
    }

    osMutexRelease(mutexBuff);
    return data;
}

void vApplicationIdleHook(void) {

}

/* USER CODE END Application */
