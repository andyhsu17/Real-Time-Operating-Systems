/*
 * gpio.h
 *
 *  Created on: Jan 24, 2020
 *      Author: Andy Hsu
 */
#include "queue.h"
#include <stdbool.h>


#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_

#define	LED0_port		gpioPortF
#define LED0_pin		0x4
#define LED0_default	false 	// Default false (0) = off, true (1) = on
// LED 1 pin is
#define LED1_port		gpioPortF
#define LED1_pin		0x5
#define LED1_default	false	// Default false (0) = off, true (1) = on

void led1_change(bool val);
void led0_change(bool val);
void gpio_open();
void GPIO_EVEN_IRQHandler(void);
void GPIO_ODD_IRQHandler(void);

#endif /* SRC_GPIO_H_ */
