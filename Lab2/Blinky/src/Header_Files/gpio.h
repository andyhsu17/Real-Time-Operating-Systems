#ifndef GPIO_H
#define GPIO_H
//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_gpio.h"

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

#define BUTTON0PORT     gpioPortF
#define BUTTON1PORT     gpioPortF
#define BUTTON0PIN      6
#define BUTTON1PIN      7
#define BUTTONDEFAULT   true


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void gpio_open(void);

#endif