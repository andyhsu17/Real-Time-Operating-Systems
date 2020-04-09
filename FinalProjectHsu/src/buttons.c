/*
 * buttons.c
 *
 *  Created on: Jan 23, 2020
 *      Author: Andy Hsu
 */
#include "buttons.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "queue.h"
#include <stdlib.h>
#include  <common/include/rtos_utils.h>

extern uint8_t bstate0;
extern uint8_t bstate1;

/*
 * button_init Initializes the clock and sets the pins for the buttons from GPIO
 * parameters: none
 * returns: none
 */
void button_init()
{
	CMU_ClockEnable(cmuClock_GPIO, true);

	GPIO_PinModeSet(BUTTONPORT, BUTTON0PIN, gpioModeInputPull, 1);

	GPIO_PinModeSet(BUTTONPORT, BUTTON1PIN, gpioModeInputPull, 1);

	GPIO_IntConfig(BUTTONPORT, BUTTON1PIN, true, true, true);
	GPIO_IntConfig(BUTTONPORT, BUTTON0PIN, true, true, true);
}


void sampleButton0()
{
	if(!GPIO_PinInGet(BUTTONPORT, BUTTON0PIN) && GPIO_PinInGet(BUTTONPORT, BUTTON1PIN))
		bstate0 = 1;
	else if(!GPIO_PinInGet(BUTTONPORT, BUTTON0PIN) && !GPIO_PinInGet(BUTTONPORT, BUTTON1PIN))
	{
		bstate0 = 0;
		bstate1 = 0;
	}
	else
		bstate0 = 0;

}


void sampleButton1()
{
	if(!GPIO_PinInGet(BUTTONPORT, BUTTON1PIN) && (GPIO_PinInGet(BUTTONPORT, BUTTON0PIN)))
		bstate1 = 1;
	else if(!GPIO_PinInGet(BUTTONPORT, BUTTON0PIN) && !GPIO_PinInGet(BUTTONPORT, BUTTON1PIN))
	{
		bstate0 = 0;
		bstate1 = 0;
	}
	else
		bstate1 = 0;
}
