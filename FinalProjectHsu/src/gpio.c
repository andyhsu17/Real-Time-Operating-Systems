/*
 * gpio.c
 *
 *  Created on: Jan 24, 2020
 *      Author: Andy Hsu
 */

#include "gpio.h"
#include "em_gpio.h"
#include "slide.h"
#include "buttons.h"
#include "queue.h"
#include <kernel/include/os.h>
#include  <common/include/rtos_utils.h>


/*
 *  gpio_open is a function that initializes drive strength and set for both LED0 and LED1
 *  parameter: none
 *  returns: none
 */
void gpio_open()
{
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, LED0_default);

	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, LED1_default);

	GPIO_IntConfig(BUTTONPORT, BUTTON0PIN, true, true, true);
	GPIO_IntConfig(BUTTONPORT, BUTTON1PIN, true, true, true);
}


/*
 * Function changes LED0 based on flags from speed and direction hazards
 * parameter: none
 *  returns: none
 */
void led0_change(bool val)
{
	if(val){
		GPIO_PinOutSet(LED0_port, LED0_pin);
	}else
		GPIO_PinOutClear(LED0_port, LED0_pin);
}


/*
 * Function changes LED1 based on flags from time and direction hazards
 * parameter: none
 *  returns: none
 */
void led1_change(bool val)
{
	if(val){
		GPIO_PinOutSet(LED1_port, LED1_pin);
	}else
		GPIO_PinOutClear(LED1_port, LED1_pin);
}










