/*
 * buttons.h
 *
 *  Created on: Jan 23, 2020
 *      Author: Andy Hsu
 */

#ifndef SRC_BUTTONS_H_
#define SRC_BUTTONS_H_

#define BUTTONPORT	gpioPortF
#define BUTTON0PIN		6

#define BUTTONPORT	gpioPortF
#define BUTTON1PIN		7

void button_init();
void sampleButton0();
void sampleButton1();

#endif /* SRC_BUTTONS_H_ */
