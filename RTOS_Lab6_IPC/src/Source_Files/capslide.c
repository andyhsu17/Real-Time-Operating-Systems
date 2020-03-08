/*
 * capslide.c
 *
 *  Created on: Jan 25, 2020
 *      Author: Andy Hsu
 */
#include "capsense.h"
#include "capslide.h"
#include "main.h"
#include "queue.h"

extern struct info_t * messageInfo;

/*************************************************************************************************
 * @brief: Logic ro check if the cap sensor is being touched or not and whether it the left or
 * 		   right
 * @param: none
 * @return: none
 ************************************************************************************************/
void capRead(){
	CAPSENSE_Sense();
	if ((CAPSENSE_getPressed(BUTTON0_0_CHANNEL)
			| CAPSENSE_getPressed(BUTTON0_1_CHANNEL))
	        && (!CAPSENSE_getPressed(BUTTON1_0_CHANNEL)
			| !CAPSENSE_getPressed(BUTTON1_1_CHANNEL)) ) {

		messageInfo->capStatus = 1; //left is zero

	}else if ((CAPSENSE_getPressed(BUTTON1_0_CHANNEL)
			| CAPSENSE_getPressed(BUTTON1_1_CHANNEL))
	        && (!CAPSENSE_getPressed(BUTTON0_0_CHANNEL)
			| !CAPSENSE_getPressed(BUTTON0_1_CHANNEL)) ) {


		messageInfo->capStatus = 2; //right is one

	}else
		messageInfo->capStatus = 0;
}
