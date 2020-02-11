/***************************************************************************//**
 * @file
 * @brief Simple LED Blink Demo for SLSTK3402A
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "capsense.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include <bsp_os.h>
#include "bsp.h"
#include "main.h"
#include "app.h"
#include "stdbool.h"
#include <cpu/include/cpu.h>
#include  <common/include/common.h>
#include  <kernel/include/os.h>
#include <kernel/include/os_trace.h>
#include <common/include/lib_def.h>
#include <common/include/rtos_utils.h>
#include <common/include/toolchains.h>

static  CPU_STK  Ex_MainStartTaskStk[EX_MAIN_START_TASK_STK_SIZE];                         /* Start Task TCB.                                      */
static  OS_TCB   Ex_MainStartTaskTCB;
static  CPU_STK  ButtonTaskStk[EX_MAIN_START_TASK_STK_SIZE];                         /* button Task TCB.                                      */
static  OS_TCB   ButtonTaskTCB;
static  CPU_STK  LEDTaskStk[EX_MAIN_START_TASK_STK_SIZE];                         /* LED Task TCB.                                      */
static  OS_TCB   LEDTaskTCB;
static  CPU_STK  SliderTaskStk[EX_MAIN_START_TASK_STK_SIZE];                         /* SLIDER Task TCB.                                      */
static  OS_TCB   SliderTaskTCB;

static  CPU_STK  IdleTaskStk[EX_MAIN_START_TASK_STK_SIZE];                         /* IDLE Task TCB.                                      */
static  OS_TCB   IdleTaskTCB;

static void Ex_MainStartTask (void  *p_arg);
static void ButtonTask (void  *p_arg);
static void LEDTask (void  *p_arg);
static void SliderTask (void  *p_arg);
static void IdleTask (void  *p_arg);

volatile bool BUTTON0PRESSED = false;
volatile bool BUTTON1PRESSED = false;
volatile bool SLIDERLEFT = false;
volatile bool SLIDERRIGHT = false;
/***************************************************************************//**
 * @brief Drive LEDs function that takes reads the state of buttons and the 
 * slider and drives the LEDs based on these global variables
 * @param none
 * @return none
 *****************************************************************************/
void driveLEDs()
{ 
    if(BUTTON1PRESSED && !BUTTON0PRESSED)       // button 1 pressed and button 0 not
	   {
		   GPIO_PinOutSet(LED0_port, LED0_pin);
	   }        
	   else if(BUTTON0PRESSED && !BUTTON1PRESSED) // button 0 pressed button 1 not
	   {
		   GPIO_PinOutSet(LED1_port, LED1_pin);
	   }
   else if (SLIDERLEFT && !SLIDERRIGHT)       // slider left 
   {
		  GPIO_PinOutSet(LED1_port, LED1_pin);
   }
   else if (SLIDERRIGHT && !SLIDERLEFT)     // slider right
   {
     GPIO_PinOutSet(LED0_port, LED0_pin);
   }
    else                                      // otherwise clear everything else
    {
		   GPIO_PinOutClear(LED0_port, LED0_pin);
		   GPIO_PinOutClear(LED1_port, LED1_pin);
	 }
}

/***************************************************************************//**
 * @brief samples button 0 and updates the button 0 global variable
 * @param  none
 * @return none
 *****************************************************************************/
void sample_button0()
{
	 if(!GPIO_PinInGet(BUTTON0PORT, BUTTON0PIN)) BUTTON0PRESSED = true;      
  else BUTTON0PRESSED = false;
}

/***************************************************************************//**
 * @brief samples button 1 and updates the button 0 global variable
 * @param  none
 * @return none
 *****************************************************************************/
void sample_button1()
{
	if(!GPIO_PinInGet(BUTTON1PORT, BUTTON1PIN)) BUTTON1PRESSED = true;
  else BUTTON1PRESSED = false;
}

/***************************************************************************//**
 * @brief samples slider and updates global variables that determine position of 
 * slider
 * @param  none
 * @return none
 *****************************************************************************/
void sampleslider()
{
  if ((CAPSENSE_getPressed(BUTTON3_CHANNEL) | CAPSENSE_getPressed(BUTTON1_CHANNEL))         // slider left is pressed slider right is not
        && !(CAPSENSE_getPressed(BUTTON0_CHANNEL) | CAPSENSE_getPressed(BUTTON2_CHANNEL))) 
  {
    SLIDERLEFT = true;
    SLIDERRIGHT = false;
  }
  else if ((CAPSENSE_getPressed(BUTTON2_CHANNEL) | CAPSENSE_getPressed(BUTTON0_CHANNEL))    // slider right is pressed and slider left is not
      && !(CAPSENSE_getPressed(BUTTON1_CHANNEL) | CAPSENSE_getPressed(BUTTON3_CHANNEL)))
  {
    SLIDERRIGHT =  true;
    SLIDERLEFT = false;
  }
  else                                                                              // if both pressed, then it doesnt matter. always turn both off
  {
    SLIDERLEFT = false;
    SLIDERRIGHT = false;
  }
}

  
/***************************************************************************//**
 * @brief main function calls all initializations and enters infinite loop that
 * either enters sleep mode or polls buttons and capacitive touch slider
 * @param  none
 * @return none
 *****************************************************************************/
int main(void)
{
  RTOS_ERR  err;
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

  /* Chip errata */
  CHIP_Init();
  EMU_DCDCInit(&dcdcInit);
  CMU_HFXOInit(&hfxoInit);
  /* Switch HFCLK to HFRCO and disable HFRCO */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  CMU_OscillatorEnable(cmuOsc_HFRCO, true, true);
  //  uint32_t clockspeed = SystemCoreClockGet();   // for debugging purposes
  
  if (SysTick_Config(SystemCoreClockGet() / 10))
  { 
    // 40MHz clock divided by 10 gives 4,000,000 (number of ticks b/w each interrupt) gives 100ms
    while (1);
  }

  OS_TRACE_INIT();
  OSInit(&err);                                               /* Initialize the Kernel.                               */
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

  /* Call application program to open / initialize all required peripheral */

  OSTaskCreate(&Ex_MainStartTaskTCB,                          /* Create the Start Task.                               */
                 "Ex Main Start Task",
                  Ex_MainStartTask,
                  DEF_NULL,
                  EX_MAIN_START_TASK_PRIO,
                 &Ex_MainStartTaskStk[0],
                 (EX_MAIN_START_TASK_STK_SIZE / 10u),
                  EX_MAIN_START_TASK_STK_SIZE,
                  0u,
                  0u,
                  DEF_NULL,
                 (OS_OPT_TASK_STK_CLR),
                 &err);
                                                                /*   Check error code.                                  */
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);


  OSStart(&err);                                              /* Start the kernel.                                    */
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);  
}

/***************************************************************************//**
 * @brief Main start task performs OS initialization as well as other initializations.
 * Also creates other 3 main tasks
 * @param  parg - unused
 * @return none
 *****************************************************************************/
void Ex_MainStartTask (void  *p_arg)
{
    RTOS_ERR  err;
    PP_UNUSED_PARAM(p_arg);                                     /* Prevent compiler warning.                            */
    BSP_TickInit();                                             /* Initialize Kernel tick source.                       */
#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)
    OSStatTaskCPUUsageInit(&err);                               /* Initialize CPU Usage.                                */               /*   Check error code.                                  */
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();                                /* Initialize interrupts disabled measurement.          */
#endif
  Common_Init(&err);                                          /* Call common module initialization example.           */
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);
  BSP_OS_Init();                                              /* Initialize the BSP. It is expected that the BSP ...  */  /* ... will register all the hardware controller to ... */


  OSTaskCreate(&IdleTaskTCB,                         // idle task
                 "Idle Task",
                  IdleTask,
                  DEF_NULL,
                  25,         // lowest priority
                 &IdleTaskStk[0],
                 (EX_MAIN_START_TASK_STK_SIZE / 10u),
                  EX_MAIN_START_TASK_STK_SIZE,
                  0u,
                  0u,
                  DEF_NULL,
                 (OS_OPT_TASK_STK_CLR),
                 &err);

  // button task
  OSTaskCreate(&ButtonTaskTCB,                          /* Create the button Task.                               */
              "Button Input Task",
               ButtonTask,
               DEF_NULL,
               BUTTONTASK_PRIO,
              &ButtonTaskStk[0],
               (EX_MAIN_START_TASK_STK_SIZE / 10u),
               EX_MAIN_START_TASK_STK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);                                 
 APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

 OSTaskCreate(&LEDTaskTCB,                          /* Create the LED Task.                               */
              "LED Input Task",
               LEDTask,
               DEF_NULL,
               LEDTASK_PRIO,
              &LEDTaskStk[0],
               (EX_MAIN_START_TASK_STK_SIZE / 10u),
               EX_MAIN_START_TASK_STK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);                                 
 APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

 OSTaskCreate(&SliderTaskTCB,                          /* Create the Slider Task.                               */
              "Slider Input Task",
               SliderTask,
               DEF_NULL,
               SLIDERTASK_PRIO,
              &SliderTaskStk[0],
               (EX_MAIN_START_TASK_STK_SIZE / 10u),
               EX_MAIN_START_TASK_STK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);                                 
 APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

    while (1)
    {
    OSTimeDly(100,                                        /*   1000 OS Ticks                                      */
              OS_OPT_TIME_DLY,                             /*   from now.                                          */
              &err);    
  }                                                           /* ... the platform manager at this moment.             */

}

/***************************************************************************//**
 * @brief button task reads both buttons and updates global variables
 * @param  parg - unused
 * @return none
 *****************************************************************************/
void ButtonTask (void  *p_arg)
{
      RTOS_ERR  err;

    app_peripheral_setup(); // function that calls cmu and gpio initializations

  while(1)
  {
    sample_button0();
    sample_button1();
    OSTimeDly(100,                                        /*   1000 OS Ticks                                      */
              OS_OPT_TIME_DLY,                             /*   from now.                                          */
              &err);    
  }
}

/***************************************************************************//**
 * @brief LED task reads global variables and drives LEDs based on boolean
 * @param  parg - unused
 * @return none
 *****************************************************************************/
void LEDTask (void  *p_arg)
{
      RTOS_ERR  err;

  while(1)
  {
        driveLEDs();
        OSTimeDly(100,                                        /*   1000 OS Ticks                                      */
              OS_OPT_TIME_DLY,                             /*   from now.                                          */
              &err);    
  }
}

/***************************************************************************//**
 * @brief slider task reads four channels of capacitive touch sensor
 *  and updates global variables
 * @param  parg - unused
 * @return none
 *****************************************************************************/
void SliderTask (void  *p_arg)
{
      RTOS_ERR  err;

    CAPSENSE_Init();

  while(1)
  {
    CAPSENSE_Sense();
    sampleslider();
    OSTimeDly(100,                                        /*   100 OS Ticks                                      */
              OS_OPT_TIME_DLY,                             /*   from now.                                          */
              &err);    
  }

}

/***************************************************************************//**
 * @brief Idle task runs when no other tasks are running. Set to lowest priority
 * and enters sleep mode
 * @param  parg - unused
 * @return none
 *****************************************************************************/
void IdleTask (void  *p_arg)
{
  while(1)
  {
    EMU_EnterEM1();
  }
}
