/*
*********************************************************************************************************
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/
#include  <stdlib.h>
#include  <stddef.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include "main.h"
#include "gpio.h"
#include "cmu.h"
#include "capsense.h"
#include "capslide.h"


#include  <bsp_os.h>
#include  "bsp.h"

#include  <cpu/include/cpu.h>
#include  <common/include/common.h>
#include  <kernel/include/os.h>
#include  <kernel/include/os_trace.h>

#include  <common/include/lib_def.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/toolchains.h>
#include "cpu_cfg.h"
#include "queue.h"


//#define LAB2_USE_INTERRUPT

/*
*********************************************************************************************************
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

#define  EX_MAIN_START_TASK_PRIO              21u
#define  EX_MAIN_START_TASK_STK_SIZE         512u


/*
*********************************************************************************************************
*********************************************************************************************************
*                                        LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/

                                                                /* Start Task Stack.                                    */
static  CPU_STK  Ex_MainStartTaskStk[EX_MAIN_START_TASK_STK_SIZE];
                                                                /* Start Task TCB.                                      */
static  OS_TCB   Ex_MainStartTaskTCB;

/* Button Task Stack.                                    */
static  CPU_STK  ButtonInputStk[EX_MAIN_START_TASK_STK_SIZE];
/* Button Task TCB.                                      */
static  OS_TCB   ButtonInputTaskTCB;

/* Slider Task Stack.                                    */
static  CPU_STK  SliderInputStk[EX_MAIN_START_TASK_STK_SIZE];
/* Slider Task TCB.                                      */
static  OS_TCB   SliderInputTaskTCB;

/* LEDOutput Task Stack.                                    */
static  CPU_STK  LEDOutputStk[EX_MAIN_START_TASK_STK_SIZE];
/* LEDOutput Task TCB.                                      */
static  OS_TCB   LEDOutputTaskTCB;

/* Idle Task Stack.                                    */
static  CPU_STK  IdleStk[EX_MAIN_START_TASK_STK_SIZE];
/* Idle Task TCB.                                      */
static  OS_TCB   IdleTaskTCB;

static OS_SEM capslideReady;

struct info_t * messageInfo;

static OS_Q messageQueue;

static OS_FLAG_GRP flags;

static OS_TMR kernelTimer;
/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

static  void  Ex_MainStartTask (void  *p_arg);

static  void  ButtonInputTask (void *p_arg);

static  void  SliderInputTask (void *p_arg);

static  void  LEDOutputTask (void *p_arg);

static  void  IdleTask (void *p_arg);

void timerCallback(void  *p_tmr, void  *p_arg);
/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*************************************************************************************************
 * @brief: Interrupt Handler for even numbered GPIOs. Used to handle when button 0 is pressed and
 * 	       not pressed
 * @param: none
 * @return: none
 ************************************************************************************************/
void GPIO_EVEN_IRQHandler(){
	uint32_t interrupt = GPIO_IntGet();
	RTOS_ERR  err;
	GPIO_IntClear(interrupt);
	 /* Set But0 Flag .                    */
	    OSFlagPost(&flags,             /*   Pointer to user-allocated event flag.    */
	                        BUT0_PRESSED,            /*   But 0 Flag bitmask.              */
	                        OS_OPT_POST_FLAG_SET,  /*   Set the flag.                            */
	                       &err);
}

/*************************************************************************************************
 * @brief: Interrupt Handler for odd numbered GPIOs. Used to handle when button 1 is pressed and
 * 	       not pressed
 * @param: none
 * @return: none
 ************************************************************************************************/
void GPIO_ODD_IRQHandler(){
	uint32_t interrupt = GPIO_IntGet();
	RTOS_ERR  err;
	GPIO_IntClear(interrupt);
	 /* Set But1 Flag .                    */
	    OSFlagPost(&flags,             /*   Pointer to user-allocated event flag.    */
	                        BUT1_PRESSED,            /*   But 1 Flag bitmask.              */
	                        OS_OPT_POST_FLAG_SET,  /*   Set the flag.                            */
	                       &err);

}

/*
*********************************************************************************************************
*                                          Ex_MainStartTask()
*
*@brief Description : This is the task that will be called by the Startup when all services are initializes
*               successfully.
*
*@param Argument(s) : p_arg   Argument passed from task creation. Unused, in this case.
*
*@return Return(s)   : None.
*********************************************************************************************************
*/

static  void  Ex_MainStartTask (void  *p_arg)
{
    RTOS_ERR  err;


    PP_UNUSED_PARAM(p_arg);                                     /* Prevent compiler warning.                            */


    BSP_TickInit();                                             /* Initialize Kernel tick source.                       */
    /* Kernel Awerness Set Up*/
    OSStatReset(&err);
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)
    OSStatTaskCPUUsageInit(&err);                               /* Initialize CPU Usage.                                */
                                                                /*   Check error code.                                  */
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();                                /* Initialize interrupts disabled measurement.          */
#endif

    Common_Init(&err);                                          /* Call common module initialization example.           */
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

    BSP_OS_Init();                                              /* Initialize the BSP. It is expected that the BSP ...  */
                                                                /* ... will register all the hardware controller to ... */
                                                                /* ... the platform manager at this moment.             */


	/* Create a periodic timer.               */
	OSTmrCreate(&kernelTimer,            						/*   Pointer to user-allocated timer.     */
				"Kernel Timer",           						/*   Name used for debugging.             */
				0,                  							/*     0 initial delay.                   */
				2,                  							/*   100 Timer Ticks period.              */
				OS_OPT_TMR_PERIODIC,  							/*   Timer is periodic.                   */
				&timerCallback,    								/*   Called when timer expires.           */
				DEF_NULL,             							/*   No arguments to callback.            */
				&err);

    /*   Check error code.                                  */
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

	OSSemCreate (&capslideReady,								/*   Pointer to user-allocated semaphore.     */
	             "Capslide Semaphore",							/*   Name used for debugging.             */
	              0,											/*   initial value             */
	              &err);

    /*   Check error code.                                  */
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

	/* Create the event flag.                          */
	OSFlagCreate(&flags,              						/*   Pointer to user-allocated event flag.         */
				 "Event Flags",             				/*   Name used for debugging.                      */
				  0,                      					/*   Initial flags, all cleared.                   */
				 &err);

	/*   Check error code.                                  */
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

	/* Create the message queue.                           */
	OSQCreate(&messageQueue,           /*   Pointer to user-allocated message queue.          */
			  "Message Queue",          /*   Name used for debugging.                          */
			   20,                     /*   Queue will have 10 messages maximum.              */
			  &err);

	/*   Check error code.                                  */
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

    OSTaskCreate(&ButtonInputTaskTCB,                          /* Create the Button Task.                               */
                 "Button Input Task",
                  ButtonInputTask,
                  DEF_NULL,
                  EX_MAIN_START_TASK_PRIO,
                 &ButtonInputStk[0],
                 (EX_MAIN_START_TASK_STK_SIZE / 10u),
                  EX_MAIN_START_TASK_STK_SIZE,
                  0u,
                  0u,
                  DEF_NULL,
                 (OS_OPT_TASK_STK_CLR),
                 &err);
                                                                /*   Check error code.                                  */
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

    OSTaskCreate(&SliderInputTaskTCB,                          /* Create the Slider Task.                               */
                 "Slider Input Task",
                  SliderInputTask,
                  DEF_NULL,
                  EX_MAIN_START_TASK_PRIO,
                 &SliderInputStk[0],
                 (EX_MAIN_START_TASK_STK_SIZE / 10u),
                  EX_MAIN_START_TASK_STK_SIZE,
                  0u,
                  0u,
                  DEF_NULL,
                 (OS_OPT_TASK_STK_CLR),
                 &err);
                                                                /*   Check error code.                                  */
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

    OSTaskCreate(&LEDOutputTaskTCB,                          /* Create the LED Task.                               */
                 "LED Output Task",
                  LEDOutputTask,
                  DEF_NULL,
                  EX_MAIN_START_TASK_PRIO,
                 &LEDOutputStk[0],
                 (EX_MAIN_START_TASK_STK_SIZE / 10u),
                  EX_MAIN_START_TASK_STK_SIZE,
                  0u,
                  0u,
                  DEF_NULL,
                 (OS_OPT_TASK_STK_CLR),
                 &err);
                                                                /*   Check error code.                                  */
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

    OSTaskCreate(&IdleTaskTCB,                          /* Create the LED Task.                               */
                 "Idle Task",
                  IdleTask,
                  DEF_NULL,
                  30,
                 &IdleStk[0],
                 (EX_MAIN_START_TASK_STK_SIZE / 10u),
                  EX_MAIN_START_TASK_STK_SIZE,
                  0u,
                  0u,
                  DEF_NULL,
                 (OS_OPT_TASK_STK_CLR),
                 &err);
                                                                /*   Check error code.                                  */
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

    while (DEF_ON) {
        //BSP_LedToggle(0);
                                                                /* Delay Start Task execution for                       */
        OSTimeDly( 1000,                                        /*   1000 OS Ticks                                      */
                   OS_OPT_TIME_DLY,                             /*   from now.                                          */
                  &err);
                                                                /*   Check error code.                                  */
        APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
    }
}

/*
*********************************************************************************************************
*                                          ButtonInputTask()
*
*@brief Description : This is the task that will be called to initialize button and run logic for checking
* 				button status
*
*@param Argument(s) : p_arg   Argument passed from task creation. Unused, in this case.
*
*@return Return(s)   : None.
*
*********************************************************************************************************
*/
static  void  ButtonInputTask (void *p_arg){
    RTOS_ERR  err;
	/* Initialize button hardware */
	buttonInit();

	while(DEF_ON){
		/* Wait until all flags are set.           */
		OSFlagPend(&flags,                /*   Pointer to user-allocated event flag. */
		                        BUT0_PRESSED | BUT1_PRESSED,             /*   Flag bitmask to match.                */
		                        0,                      /*   Wait for 100 OS Ticks maximum.        */
								OS_OPT_PEND_FLAG_SET_ANY    |/*   Wait until any flags are set and      */
		                        OS_OPT_PEND_BLOCKING     |/*    task will block and                  */
		                        OS_OPT_PEND_FLAG_CONSUME, /*    function will clear the flags.       */
		                        DEF_NULL,                 /*   Timestamp is not used.                */
		                       &err);


		  messageInfo = malloc(sizeof(struct info_t));

		  sampleButton0();
		  sampleButton1();

		  /* Send message to the waiting task.                */
		  OSQPost(&messageQueue,                /*   Pointer to user-allocated message queue.       */
				  messageInfo,                 /*   The message is a pointer to the APP_MESSAGE.   */
				  sizeof(struct info_t *),  /*   Size of the message is the size of a pointer.  */
				   OS_OPT_POST_FIFO,            /*   Add message at the end of the queue.           */
				  &err);
	          /*   Check error code.                                  */
		  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
          /* Delay Task execution for                       */
		  OSTimeDly( 100,                                        /*   1000 OS Ticks                                      */
				  OS_OPT_TIME_DLY,                             /*   from now.                                          */
				  &err);
          /*   Check error code.                                  */
		  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
	}
}

/*
*********************************************************************************************************
*                                          SliderInputTask()
*
* @brief Description : This is the task that will be called to initialize slider and run logic for checking
* 				button status
*
* @param Argument(s) : p_arg   Argument passed from task creation. Unused, in this case.
*
* @return Return(s)   : None.
*
* Notes       : None.
*********************************************************************************************************
*/
static  void  SliderInputTask (void *p_arg){
    RTOS_ERR  err;
	/* Initialize slider hardware */
    CAPSENSE_Init();

    /* Start the timer.                        */
       OSTmrStart(&kernelTimer,  		/*   Pointer to user-allocated timer.      */
                  &err);

       /*   Check error code.                                  */
       APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

	while(DEF_ON){
		OSSemPend (&capslideReady,							/*   Pointer to user-allocated semaphore.     */
		           0,										/*   Timeout      */
				   OS_OPT_PEND_BLOCKING,					/*   Type of Pend     */
		           DEF_NULL,									/*   timestamp     */
		           &err);
		  messageInfo = malloc(sizeof(struct info_t));

		  capRead();

		  /* Send message to the waiting task.                */
		  OSQPost(&messageQueue,                /*   Pointer to user-allocated message queue.       */
				  messageInfo,                 /*   The message is a pointer to the APP_MESSAGE.   */
				  sizeof(struct info *),  /*   Size of the message is the size of a pointer.  */
				   OS_OPT_POST_FIFO,            /*   Add message at the end of the queue.           */
				  &err);
		  	          /*   Check error code.                                  */
		  		  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
          /* Delay Task execution for                       */
		  OSTimeDly( 100,                                        /*   1000 OS Ticks                                      */
				  OS_OPT_TIME_DLY,                             /*   from now.                                          */
				  &err);
          /*   Check error code.                                  */
		  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
	}
}

/*
*********************************************************************************************************
*                                          LedOutputTask()
*
* @brief Description : This is the task that will be called to initialize slider and run logic for checking
* 				button status
*
* @param Argument(s) : p_arg   Argument passed from task creation. Unused, in this case.
*
* @return Return(s)   : None.
*
*********************************************************************************************************
*/
static  void  LEDOutputTask (void *p_arg){
    RTOS_ERR  err;
    struct info_t * msg;
    OS_MSG_SIZE size;
	/* Initialize LED hardware */
    ledInit();

	while(DEF_ON){
		/* Wait until task receives a message.        */
		msg = (struct info_t *) OSQPend(&messageQueue,          /*   Pointer to user-allocated message queue. */
							 0,                   /*   Wait for 100 OS Ticks maximum.           */
							 OS_OPT_PEND_BLOCKING,  /*   Task will block.                         */
							&size,              /*   Will contain size of message in bytes.   */
							 DEF_NULL,              /*   Timestamp is not used.                   */
							&err);
        /*   Check error code.                                  */
		  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
   		  led0Drive(msg);
		  led1Drive(msg);
		  free(msg);
          /* Delay Task execution for                       */
		  OSTimeDly( 100,                                        /*   1000 OS Ticks                                      */
				  OS_OPT_TIME_DLY,                             /*   from now.                                          */
				  &err);
          /*   Check error code.                                  */
		  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
	}
}

/*
*********************************************************************************************************
*                                          IdleTask()
*
*@brief Description : Idle Task
*
*@param Argument(s) : p_arg   Argument passed from task creation. Unused, in this case.
*
*@return Return(s)   : None.
*
* Notes       : None.
*********************************************************************************************************
*/
static  void  IdleTask (void *p_arg){
	while(DEF_ON){
		EMU_EnterEM1();
	}
}

/*
*********************************************************************************************************
*                                          timerCallback()
*
*@brief Description : Callback function for timmer to release semaphore
*
*@param Argument(s) : None.
*
*@return Return(s)   : None.
*
* Notes       : None.
*********************************************************************************************************
*/
void timerCallback(void  *p_tmr, void  *p_arg){
	RTOS_ERR  err;

	OSSemPost (&capslideReady,							/*   Pointer to user-allocated semaphore.     */
						OS_OPT_POST_1,					/*   Type of post     */
			            &err);

    /*   Check error code.                                  */
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
}

/*************************************************************************************************
 * @brief Interrupt Handler for the SysTick timer. Used to periodically check the state
 * @param none
 * @return none
 ************************************************************************************************/
/*void SysTick_Handler(void)
{
	capRead();
	led0Drive();
	led1Drive();
}*/

/*************************************************************************************************
 * @brief: Logic for turning LED0 on or off
 * @param: none
 * @return: none
 ************************************************************************************************/
void led0Drive(struct info_t * msg){
	if(msg->button0Status || msg->capStatus == 1){
		GPIO_PinOutSet(LED0_port, LED0_pin);
	}else
		GPIO_PinOutClear(LED0_port, LED0_pin);
}

/*************************************************************************************************
 * @brief Logic for turning LED1 on or off
 * @param none
 * @return none
 ************************************************************************************************/
void led1Drive(struct info_t * msg){
	if(msg->button1Status || msg->capStatus == 2){
		GPIO_PinOutSet(LED1_port, LED1_pin);
	}else
		GPIO_PinOutClear(LED1_port, LED1_pin);
}

/*************************************************************************************************
 * @brief main function where code is initialized and stays idle and is interrupted from.
 * @param none
 * @return none
 ************************************************************************************************/
int main(void)

{
    RTOS_ERR  err;


    BSP_SystemInit();                                           /* Initialize System.                                   */
    CPU_Init();                                                 /* Initialize CPU.                                      */
    CPU_TS_TmrInit();
    OS_TRACE_INIT();
    OSInit(&err);                                               /* Initialize the Kernel.                               */
                                                                /*   Check error code.                                  */
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);


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
                                                                /*   Check error code.                                  */
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);


  /* Chip errata */
  CHIP_Init();
#ifndef LAB2_USE_INTERRUPT
#else
  /* Setup SysTick timer for 100 msec interrupts  */
  if (SysTick_Config(SystemCoreClockGet() / 10)) {
	  while (1) ;
  }
#endif


#ifdef LAB3_Part1_ON
  /* Infinite loop */
  while (1) {

#ifndef LAB2_USE_INTERRUPT
	  sampleButton0();
	  sampleButton1();
	  capRead();
	  led0Drive();
	  led1Drive();
#else
	  EMU_EnterEM1();
#endif //LAB2_USE_INTERRUPT

  }
#endif //LAB3_Part1_ON

  return 1;
}


