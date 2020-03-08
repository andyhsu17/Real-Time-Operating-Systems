/*
 * gpio.h
 *
 *  Created on: Jan 23, 2020
 *      Author: Andy Hsu
 */

#ifndef SRC_HEADER_FILES_GPIO_H_
#define SRC_HEADER_FILES_GPIO_H_

//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_gpio.h"
#include "em_cmu.h"

//***********************************************************************************
// defined files
//***********************************************************************************

// LED 0 pin is
#define	LED0_port		gpioPortF
#define LED0_pin		4
#define LED0_default	false 	// Default false (0) = off, true (1) = on

// LED 1 pin is
#define LED1_port		gpioPortF
#define LED1_pin		5
#define LED1_default	false	// Default false (0) = off, true (1) = on

// Button 0
#define BUT0_port		gpioPortF
#define BUT0_pin		6

// Button 0
#define BUT1_port		gpioPortF
#define BUT1_pin		7

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void gpioInit(void);
void buttonInit(void);
void ledInit(void);
void sampleButton0();
void sampleButton1();

void GPIO_EVEN_IRQHandler();
void GPIO_ODD_IRQHandler();

#endif /* SRC_HEADER_FILES_GPIO_H_ */
