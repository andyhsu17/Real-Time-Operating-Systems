/*
 * capslide.c
 *
 *  Created on: Jan 25, 2020
 *      Author: Andy Hsu
 */
#include  <kernel/include/os.h>
#include "capsense.h"
#include "capslide.h"
#include "main.h"
#include "queue.h"

//extern struct info_t * buttonInfo;
extern directionInfo dirInfo;
extern OS_MUTEX directionMutex;
extern OS_FLAG_GRP vehicleFlag;

/*************************************************************************************************
 * @brief: Logic ro check if the cap sensor is being touched or not and whether it the left or
 * 		   right
 * @param: none
 * @return: none
 ************************************************************************************************/
bool capRead(void){
	bool changed = false;
	RTOS_ERR err;
	CAPSENSE_Sense();

	if ((CAPSENSE_getPressed(BUTTON0_0_CHANNEL) &&
			!CAPSENSE_getPressed(BUTTON0_1_CHANNEL)) &&
	        !CAPSENSE_getPressed(BUTTON1_0_CHANNEL) &&
			!CAPSENSE_getPressed(BUTTON1_1_CHANNEL)) 
	{
		OSMutexPend(&directionMutex,                // acquire lock for speed setpoint struct
			0,
			OS_OPT_PEND_BLOCKING,
			NULL,
			&err);
		changed = (dirInfo.dir != HARDLEFT) ? true : false;
		if(changed)
		{
			dirInfo.time = 0;
		}
		else
		{
			dirInfo.time++;
		}
		dirInfo.leftTurns++;
		dirInfo.dir = HARDLEFT;
		OSMutexPost(&directionMutex,
					OS_OPT_POST_NONE,
					&err);
	}
	else if ((!CAPSENSE_getPressed(BUTTON0_0_CHANNEL) &&
			CAPSENSE_getPressed(BUTTON0_1_CHANNEL)) &&
	        !CAPSENSE_getPressed(BUTTON1_0_CHANNEL) &&
			!CAPSENSE_getPressed(BUTTON1_1_CHANNEL)) 
	{
		OSMutexPend(&directionMutex,                // acquire lock for speed setpoint struct
					0,
					OS_OPT_PEND_BLOCKING,
					NULL,
					&err);
		changed = (dirInfo.dir != LEFT) ? true : false;
		if(changed)
		{
			dirInfo.time = 0;
		}
		else
		{
			dirInfo.time++;
		}
		dirInfo.leftTurns++;
		dirInfo.dir = LEFT;
		OSMutexPost(&directionMutex,
					OS_OPT_POST_NONE,
					&err);
	}
	else if ((!CAPSENSE_getPressed(BUTTON0_0_CHANNEL) &&
			!CAPSENSE_getPressed(BUTTON0_1_CHANNEL)) &&
	        CAPSENSE_getPressed(BUTTON1_0_CHANNEL) &&
			!CAPSENSE_getPressed(BUTTON1_1_CHANNEL)) 
	{
		OSMutexPend(&directionMutex,                // acquire lock for speed setpoint struct
					0,
					OS_OPT_PEND_BLOCKING,
					NULL,
					&err);
		changed = (dirInfo.dir != RIGHT) ? true : false;
		if(changed)
		{
			dirInfo.time = 0;
		}
		else
		{
			dirInfo.time++;
		}
		dirInfo.rightTurns++;
		dirInfo.dir = RIGHT;
		OSMutexPost(&directionMutex,
					OS_OPT_POST_NONE,
					&err);
	}
	else if ((!CAPSENSE_getPressed(BUTTON0_0_CHANNEL) &&
			!CAPSENSE_getPressed(BUTTON0_1_CHANNEL)) &&
	        !CAPSENSE_getPressed(BUTTON1_0_CHANNEL) &&
			CAPSENSE_getPressed(BUTTON1_1_CHANNEL)) 
	{
		OSMutexPend(&directionMutex,                // acquire lock for speed setpoint struct
					0,
					OS_OPT_PEND_BLOCKING,
					NULL,
					&err);
		changed = (dirInfo.dir != HARDRIGHT) ? true : false;
		if(changed)
		{
			dirInfo.time = 0;
		}
		else
		{
			dirInfo.time++;
		}
		dirInfo.rightTurns++;
		dirInfo.dir = HARDRIGHT;
		OSMutexPost(&directionMutex,
					OS_OPT_POST_NONE,
					&err);
	}
	else 
	{
		OSMutexPend(&directionMutex,                // acquire lock for speed setpoint struct
					0,
					OS_OPT_PEND_BLOCKING,
					NULL,
					&err);
		changed = (dirInfo.dir != STRAIGHT) ? true : false;
		dirInfo.time = 0;
		dirInfo.dir = STRAIGHT;
		OSMutexPost(&directionMutex,
					OS_OPT_POST_NONE,
					&err);
	}
	OSMutexPend(&directionMutex,                // acquire lock for speed setpoint struct
			0,
			OS_OPT_PEND_BLOCKING,
			NULL,
			&err);
	if(dirInfo.time > 5 && dirInfo.dir != STRAIGHT)
		OSFlagPost(&vehicleFlag,             /*   Pointer to user-allocated event flag.    */
					DIRECTIONCHANGE,            /*   But 0 Flag bitmask.              */
					OS_OPT_POST_FLAG_SET,  /*   Set the flag.                            */
					&err);
	OSMutexPost(&directionMutex,
			OS_OPT_POST_NONE,
			&err);
	return changed;
}
