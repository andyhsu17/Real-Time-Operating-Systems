/*
 * slide.c
 *
 *  Created on: Jan 24, 2020
 *      Author: Andy Hsu
 */

#include "slide.h"
#include "capsense.h"
#include "em_acmp.h"
#include "queue.h"
#include <kernel/include/os.h>
#include  <common/include/rtos_utils.h>

extern uint8_t curr_direction;
extern uint8_t prev_dir;
extern uint32_t left_cnt; // count of left turns
extern uint32_t right_cnt; // count of right turns
extern uint32_t dir_ms;


void capSense(void)
{
	CAPSENSE_Sense();

	if(CAPSENSE_getPressed(CAP_CH0)
			&& (!CAPSENSE_getPressed(CAP_CH1)
					| !CAPSENSE_getPressed(CAP_CH2) | !CAPSENSE_getPressed(CAP_CH3)))
	{
		left_cnt++;
		curr_direction = HARD_LEFT;
	}
	else if(CAPSENSE_getPressed(CAP_CH1)
			&& (!CAPSENSE_getPressed(CAP_CH0)
					| !CAPSENSE_getPressed(CAP_CH2) | !CAPSENSE_getPressed(CAP_CH3)))
	{
		left_cnt++;
		curr_direction = SOFT_LEFT;
	}
	else if(CAPSENSE_getPressed(CAP_CH2)
			&& (!CAPSENSE_getPressed(CAP_CH3)
					| !CAPSENSE_getPressed(CAP_CH0) | !CAPSENSE_getPressed(CAP_CH1)))
	{
		right_cnt++;
		curr_direction = SOFT_RIGHT;
	}
	else if(CAPSENSE_getPressed(CAP_CH3)
			&& (!CAPSENSE_getPressed(CAP_CH2)
					| !CAPSENSE_getPressed(CAP_CH0) | !CAPSENSE_getPressed(CAP_CH1)))
	{
		right_cnt++;
		curr_direction = HARD_RIGHT;
	}
	else
	{
		curr_direction = STR8;
	}

	if(prev_dir == curr_direction)
		dir_ms++;
	else
		dir_ms = 0;
}

