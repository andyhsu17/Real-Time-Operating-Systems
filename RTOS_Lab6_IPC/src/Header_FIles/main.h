//***********************************************************************************
// Include files
//***********************************************************************************

#include <stdint.h>
#include <stdbool.h>

//***********************************************************************************
// defined files
//***********************************************************************************



//***********************************************************************************
// global variables
//***********************************************************************************
volatile bool button0Status;
volatile bool button1Status;
volatile uint32_t capStatus;

//***********************************************************************************
// function prototypes
//***********************************************************************************
void led0Drive();
void led1Drive();
