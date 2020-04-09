/*
 * slide.h
 *
 *  Created on: Jan 24, 2020
 *      Author: Andy Hsu
 */
#ifndef SRC_SLIDE_H_
#define SRC_SLIDE_H_

#define CAP_CH0		0
#define CAP_CH1		1
#define CAP_CH2		2
#define CAP_CH3		3

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief ENUM that holds the direction on cap sense
//----------------------------------------------------------------------------------------------------------------------------------
enum {
	STR8			= (1u << 0),
	HARD_LEFT 		= (1u << 1),
	SOFT_LEFT		= (1u << 2),
	SOFT_RIGHT		= (1u << 3),
	HARD_RIGHT		= (1u << 4),
};

void cap_read();


#endif /* SRC_SLIDE_H_ */
