/*
 * gpio.c
 *
 *  Created on: Jan 23, 2020
 *      Author: Andy Hsu
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include <stdlib.h>
#include "gpio.h"
#include "main.h"
#include  <kernel/include/os.h>
#include "queue.h"


//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************
// extern struct info_t * buttonInfo;
extern node_t * head;
//***********************************************************************************
// function prototypes
//***********************************************************************************


//***********************************************************************************
// functions
//***********************************************************************************

/*************************************************************************************************
 * @brief: GPIO initialization
 * @param: none
 * @return: none
 ************************************************************************************************/
void gpioInit(void){
	// Set LED ports to be standard output drive with default off (cleared)
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, LED0_default);

	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, LED1_default);

	// Set Button ports to be standard input
	GPIO_PinModeSet(BUT0_port, BUT0_pin, gpioModeInputPull, true);
	GPIO_IntConfig(BUT0_port, BUT0_pin, true, true, true);

	GPIO_PinModeSet(BUT1_port, BUT1_pin, gpioModeInputPull, true);
	GPIO_IntConfig(BUT0_port, BUT1_pin, true, true, true);

	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/*************************************************************************************************
 * @brief: LED initialization
 * @param: none
 * @return: none
 ************************************************************************************************/
void ledInit(void){
	// Set LED ports to be standard output drive with default off (cleared)
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, LED0_default);

	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, LED1_default);
}

/*************************************************************************************************
 * @brief: Button initialization
 * @param: none
 * @return: none
 ************************************************************************************************/
void buttonInit(void){
	// Peripheral clocks enabled
	CMU_ClockEnable(cmuClock_GPIO, true);

	// Set Button ports to be standard input
	GPIO_PinModeSet(BUT0_port, BUT0_pin, gpioModeInputPull, true);
	GPIO_IntConfig(BUT0_port, BUT0_pin, true, true, true);

	GPIO_PinModeSet(BUT1_port, BUT1_pin, gpioModeInputPull, true);
	GPIO_IntConfig(BUT0_port, BUT1_pin, true, true, true);

	// Interrupts enabled in vector
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);

}

/*************************************************************************************************
 * @brief: Code to check if whether the button 0 is pressed or not
 * @param: none
 * @return: none
 ************************************************************************************************/
void sampleButtons(void){
	if(GPIO_PinInGet(BUT1_port, BUT1_pin) && !GPIO_PinInGet(BUT0_port, BUT0_pin))
	{
		info_t * buttonInfo = (info_t*)malloc(sizeof(info_t));
		buttonInfo->button0Pressed = true;
		buttonInfo->button1Pressed = false;
		push(&head, buttonInfo);
	}
	else if(!GPIO_PinInGet(BUT1_port, BUT1_pin) && GPIO_PinInGet(BUT0_port, BUT0_pin))
	{
		info_t * buttonInfo = (info_t*)malloc(sizeof(info_t));
		buttonInfo->button0Pressed = false;
		buttonInfo->button1Pressed = true;
		push(&head, buttonInfo);
	}
	// else
	// {
	// 	buttonInfo->button0Pressed = false;
	// 	buttonInfo->button1Pressed = false;
	// 	push(&head, buttonInfo);
	// }
}

// /*************************************************************************************************
//  * @brief: Code to check if whether the button 1 is pressed or not
//  * @param: none
//  * @return: none
//  ************************************************************************************************/
// void sampleButton1(){
// 	if(!GPIO_PinInGet(BUT1_port, BUT1_pin) && GPIO_PinInGet(BUT0_port, BUT0_pin))
// 		buttonInfo->button1Pressed = true;
// 	else if(!GPIO_PinInGet(BUT1_port, BUT1_pin) && !GPIO_PinInGet(BUT0_port, BUT0_pin)){
// 		buttonInfo->button0Pressed = false;
// 		buttonInfo->button1Pressed = false;
// 	}else
// 		buttonInfo->button1Pressed = false;
// }


