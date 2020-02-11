#ifndef MAIN_H
#define MAIN_H
//***********************************************************************************
// Include files
//***********************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <kernel/include/os.h>

//***********************************************************************************
// defines
//***********************************************************************************
//#define LAB2_USE_INTERRUPT
#define BUTTON2_CHANNEL     1   
#define BUTTON3_CHANNEL     2
#define  EX_MAIN_START_TASK_PRIO              21u
#define  EX_MAIN_START_TASK_STK_SIZE         512u

#define BUTTONTASK_PRIO                     21u
#define LEDTASK_PRIO                        21u
#define SLIDERTASK_PRIO                     21u


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************

void driveLEDs();
void sample_button0();
void sample_button1();
void sampleslider();


#endif
