#ifndef MAIN_H
#define MAIN_H

//***********************************************************************************
// Include files
//***********************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <kernel/include/os.h>

// structs and enums
enum direction
{
    HARDLEFT,
	LEFT,
	RIGHT,
	HARDRIGHT,
    STRAIGHT,
};

typedef struct speedInfo
{
    uint8_t speed;
    uint8_t increments;
    uint8_t decrements;
}speedInfo;

typedef struct directionInfo
{
	uint8_t dir;
	uint8_t time;
	uint8_t leftTurns;
	uint8_t rightTurns;
}directionInfo;

// enum that holds constants for flag definitions
enum flag
{
    SPEEDCHANGE = 1U << 0,
    DIRECTIONCHANGE = 1U << 1 ,
};

enum ledFlag
{
    OVER45 = 1U << 0,
    OVER75 = 1U << 1,
    COLLISION = 1U << 2,
    NOTOVER45 = 1U << 3,
    NOTOVER75 = 1U << 4,
    NOTCOLLISION = 1U << 5,
};

//***********************************************************************************
// defines
//***********************************************************************************



//***********************************************************************************
// global variables
//***********************************************************************************
volatile bool button0Status;
volatile bool button1Status;
volatile uint32_t capStatus;
//extern directionInfo dirInfo;
//extern OS_MUTEX directionMutex;

//***********************************************************************************
// function prototypes
//***********************************************************************************
void led0Drive();
void led1Drive();

#endif // MAIN_H
