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

#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"

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
*                                        GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/

                                                                /* Start Task Stack.                                    */
static  CPU_STK  Ex_MainStartTaskStk[EX_MAIN_START_TASK_STK_SIZE];
static  OS_TCB   Ex_MainStartTaskTCB;

static  CPU_STK  speedSetpointStk[EX_MAIN_START_TASK_STK_SIZE];
static  OS_TCB   speedSetpointTaskTCB;

static  CPU_STK  vehicleDirectionStk[EX_MAIN_START_TASK_STK_SIZE];
static  OS_TCB   vehicleDirectionTaskTCB;

static  CPU_STK  vehicleMonitorStk[EX_MAIN_START_TASK_STK_SIZE];
static  OS_TCB   vehicleMonitorTCB;

static  CPU_STK  LEDOutputStk[EX_MAIN_START_TASK_STK_SIZE];
static  OS_TCB   LEDOutputTaskTCB;

static  CPU_STK  IdleStk[EX_MAIN_START_TASK_STK_SIZE];
static  OS_TCB   IdleTaskTCB;

static  CPU_STK  lcdDisplayStk[EX_MAIN_START_TASK_STK_SIZE];
static  OS_TCB   lcdDisplayTCB;

static OS_SEM timerSem;


static OS_Q messageQueue;


static OS_TMR kernelTimer;

// synchronization variables
static OS_SEM queueSem;
static OS_MUTEX speedMutex;
OS_MUTEX directionMutex;
OS_FLAG_GRP vehicleFlag;
static OS_FLAG_GRP LEDFlag;

// general 
struct info_t * buttonInfo;
node_t * head; 
directionInfo dirInfo;
volatile speedInfo spdInfo;

/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

static  void  Ex_MainStartTask (void  *p_arg);

static  void  speedSetpointTask (void *p_arg);

static  void  vehicleDirectionTask (void *p_arg);

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
    __disable_irq();
	uint32_t interrupt = GPIO_IntGet();
	RTOS_ERR  err;
	GPIO_IntClear(interrupt);
    sampleButtons();
	OSSemPost(&queueSem,							/*   Pointer to user-allocated semaphore.     */
                OS_OPT_POST_1,					/*   Type of post     */
                &err);
    __enable_irq();
}

/*************************************************************************************************
 * @brief: Interrupt Handler for odd numbered GPIOs. Used to handle when button 1 is pressed and
 * 	       not pressed
 * @param: none
 * @return: none
 ************************************************************************************************/
void GPIO_ODD_IRQHandler(){
    __disable_irq();
	uint32_t interrupt = GPIO_IntGet();
	RTOS_ERR  err;
	GPIO_IntClear(interrupt);
    sampleButtons();
    OSSemPost(&queueSem,							/*   Pointer to user-allocated semaphore.     */
                OS_OPT_POST_1,					/*   Type of post     */
                &err);
    __enable_irq();
}


/*
*********************************************************************************************************
*                                          speedSetpointTask()
*
*@brief Description : This task initializes the buttons and pends on a semaphore from the GPIO ISR
                        It will read from the FIFO and notifies vehicle monitor task of change in speed
*@param Argument(s) : p_arg   Argument passed from task creation
*
*@return Return(s)   : None.
*
*********************************************************************************************************
*/
static  void  speedSetpointTask (void *p_arg){
    RTOS_ERR  err;
	/* Initialize button hardware */
	buttonInit();
    buttonInfo = malloc(sizeof(struct info_t));
    OSMutexCreate(&speedMutex,
    			  "Speed Mutex",
				  &err);
    info_t *msg;
    spdInfo.speed = spdInfo.increments = spdInfo.decrements = 0;
	while(DEF_ON)
    {
		/* Wait until all flags are set.           */
//		OSFlagPend(&flags,                /*   Pointer to user-allocated event flag. */
//		                        BUT0_PRESSED | BUT1_PRESSED,             /*   Flag bitmask to match.                */
//		                        0,                      /*   Wait for 100 OS Ticks maximum.        */
//								OS_OPT_PEND_FLAG_SET_ANY    |/*   Wait until any flags are set and      */
//		                        OS_OPT_PEND_BLOCKING     |/*    task will block and                  */
//		                        OS_OPT_PEND_FLAG_CONSUME, /*    function will clear the flags.       */
//		                        DEF_NULL,                 /*   Timestamp is not used.                */
//		                       &err);

    OSSemPend(&queueSem,                        // wait on post from GPIO ISR
                0,
                OS_OPT_PEND_BLOCKING,
                NULL,
                &err);
    __disable_irq();

    if(!is_empty(&head))
    {
        msg = head->message;
        if(msg->button0Pressed && !msg->button1Pressed)
        {
            OSFlagPost(&vehicleFlag,             /*   Pointer to user-allocated event flag.    */
                                SPEEDCHANGE,            /*   But 0 Flag bitmask.              */
                                OS_OPT_POST_FLAG_SET,  /*   Set the flag.                            */
                            &err);
            
            pop(&head);
            OSMutexPend(&speedMutex,                // acquire lock for speed setpoint struct
                        0,
                        OS_OPT_PEND_BLOCKING,
                        NULL,
                        &err);
            spdInfo.speed += 5;
            spdInfo.increments++;
            OSMutexPost(&speedMutex,
                        OS_OPT_POST_NONE,
                        &err);
        }
        else if(msg->button1Pressed && !msg->button0Pressed)
        {
            OSFlagPost(&vehicleFlag,             /*   Pointer to user-allocated event flag.    */
                                SPEEDCHANGE,            /*   But 0 Flag bitmask.              */
                                OS_OPT_POST_FLAG_SET,  /*   Set the flag.                            */
                            &err);
            pop(&head);
            OSMutexPend(&speedMutex,                // acquire lock for speed setpoint struct
                        0,
                        OS_OPT_PEND_BLOCKING,
                        NULL,
                        &err);
            if(spdInfo.speed > 5)
                spdInfo.speed -= 5;
            spdInfo.decrements++;
            OSMutexPost(&speedMutex,
                        OS_OPT_POST_NONE,
                        &err);
        }       // dont send a flag when both buttons or no buttons are pressed (this may be bad)
                // also making critical section reading from fifo and making speed data structure changes
    }
    __enable_irq();


        /* Send message to the waiting task.                */
        // OSQPost(&messageQueue,                /*   Pointer to user-allocated message queue.       */
        //         buttonInfo,                 /*   The message is a pointer to the APP_MESSAGE.   */
        //         sizeof(struct info_t *),  /*   Size of the message is the size of a pointer.  */
        //         OS_OPT_POST_FIFO,            /*   Add message at the end of the queue.           */
        //         &err);
            /*   Check error code.                                  */
        APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
	}
}

/*
*********************************************************************************************************
*                                          vehicleDirectionTask()
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
static  void  vehicleDirectionTask (void *p_arg){
    RTOS_ERR  err;
	/* Initialize slider hardware */
    CAPSENSE_Init();
    OSMutexCreate(&directionMutex,
       			  "Direction Mutex",
   				  &err);

    /* Start the timer.                        */
    OSTmrStart(&kernelTimer,  		/*   Pointer to user-allocated timer.      */
                &err);

    /*   Check error code.                                  */
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
    buttonInfo = malloc(sizeof(struct info_t));
    bool changed;
    dirInfo.dir = STRAIGHT;
    dirInfo.time = dirInfo.leftTurns = dirInfo.rightTurns = 0;
	while(DEF_ON)
    {
		OSSemPend (&timerSem,							/*   Pointer to user-allocated semaphore.     */
		           0,										/*   Timeout      */
				   OS_OPT_PEND_BLOCKING,					/*   Type of Pend     */
		           DEF_NULL,									/*   timestamp     */
		           &err);
        changed = capRead();
        if(changed)
            OSFlagPost(&vehicleFlag,             /*   Pointer to user-allocated event flag.    */
                        DIRECTIONCHANGE,            /*   But 0 Flag bitmask.              */
                        OS_OPT_POST_FLAG_SET,  /*   Set the flag.                            */
                        &err);
	}
}
/*	*********************************************************************************************************
 *
 * vehicleMonitorTask()
 *@brief
 *
 *@param
 *
 *@return		none
*********************************************************************************************************h
 * */
static void vehicleMonitorTask(void * p_arg)
{
	RTOS_ERR err;
    	/* Create the event flag.                          */
	OSFlagCreate(&vehicleFlag,              						/*   Pointer to user-allocated event flag.         */
				 "Vehicle Flags",             				/*   Name used for debugging.                      */
				  0,                      					/*   Initial flags, all cleared.                   */
				 &err);
    OS_FLAGS values;
    uint8_t currSpd = 0;
    uint8_t currDir = STRAIGHT;
    uint32_t currTime = 0;
    bool preViolation45 = false;
    bool currViolation45 = false;
    bool preViolation75 = false;
    bool currViolation75 = false;
    bool preViolationColl = false;
    bool currViolationColl = false;
    while(1)
    {
        OSFlagPend(&vehicleFlag,                /*   Pointer to user-allocated event flag. */
                SPEEDCHANGE | DIRECTIONCHANGE,             /*   Flag bitmask to match.                */
                0,                      /*   Wait for 100 OS Ticks maximum.        */
                OS_OPT_PEND_FLAG_SET_ANY   +/*   Wait until any flags are set and      */
                OS_OPT_PEND_BLOCKING,    /*    task will block and                  */
                DEF_NULL,                 /*   Timestamp is not used.                */
                &err);
        values = OSFlagPendGetFlagsRdy(&err);
        OSFlagPost(&vehicleFlag,                // clear flags that triggered this task
                    values,
                    OS_OPT_POST_FLAG_CLR,
                    &err);

        OSMutexPend(&speedMutex,
                    0,
                    OS_OPT_PEND_BLOCKING,
                    NULL,
                    &err);
        currSpd = spdInfo.speed;
        OSMutexPost(&speedMutex,
                    OS_OPT_POST_NONE,
                    &err);

        OSMutexPend(&directionMutex,
                    0,
                    OS_OPT_PEND_BLOCKING,
                    NULL,
                    &err);
        currDir = dirInfo.dir;
        currTime = dirInfo.time;
        OSMutexPost(&directionMutex,
                    OS_OPT_POST_NONE,
                    &err);
        if(currSpd > 75)
        {
            currViolation75 = true;
        }
        else currViolation75 = false;
        if(currSpd > 45 && currDir != STRAIGHT)
        {
            currViolation45 = true;
        }
        else currViolation45 = false;
        if(currTime > 5 && currDir != STRAIGHT)
        {
            currViolationColl = true;
        }
        else currViolationColl = false;
        if(preViolation45 != currViolation45)
        {
            if(currViolation45)
                OSFlagPost(&LEDFlag,       
                            OVER45,
                            OS_OPT_POST_FLAG_SET,
                            &err);
            else 
                OSFlagPost(&LEDFlag,       
                            NOTOVER45,
                            OS_OPT_POST_FLAG_SET,
                            &err);
        }
        if(preViolation75 != currViolation75)
        {
            if(currViolation75)
                OSFlagPost(&LEDFlag,       
                            OVER75,
                            OS_OPT_POST_FLAG_SET,
                            &err);
            else
                OSFlagPost(&LEDFlag,       
                            NOTOVER75,
                            OS_OPT_POST_FLAG_SET,
                            &err);
        }
        if(preViolationColl != currViolationColl)
        {
            if(currViolationColl)
                OSFlagPost(&LEDFlag,       
                            COLLISION,
                            OS_OPT_POST_FLAG_SET,
                            &err);
            else
                OSFlagPost(&LEDFlag,       
                            NOTCOLLISION,
                            OS_OPT_POST_FLAG_SET,
                            &err);
        }
        preViolation45 = currViolation45;
        preViolation75 = currViolation75;
        preViolationColl = currViolationColl;
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
    OS_FLAGS values;
    OSFlagCreate(&LEDFlag,              						/*   Pointer to user-allocated event flag.         */
                "LED Flags",             				/*   Name used for debugging.                      */
                0,                      					/*   Initial flags, all cleared.                   */
                &err);
	/* Initialize LED hardware */
    ledInit();

	while(DEF_ON)
    {
		/* Wait until task receives a message.        */
        OSFlagPend(&LEDFlag,                /*   Pointer to user-allocated event flag. */
                    OVER45 | OVER75 | COLLISION |
                    NOTOVER45 | NOTOVER75 | NOTCOLLISION,             /*   Flag bitmask to match.                */
                    0,                      /*   Wait for 100 OS Ticks maximum.        */
                    OS_OPT_PEND_FLAG_SET_ANY  +/*   Wait until any flags are set and      */
                    OS_OPT_PEND_BLOCKING,    /*    task will block and                  */
                    DEF_NULL,                 /*   Timestamp is not used.                */
                    &err);
        values = OSFlagPendGetFlagsRdy(&err);
        OSFlagPost(&LEDFlag,                // clear flags that triggered this task
                    values,
                    OS_OPT_POST_FLAG_CLR,
                    &err);
        if(values & OVER45)
        {
            GPIO_PinOutSet(LED0_port, LED0_pin);
        }
        if(values & OVER75)
        {
            GPIO_PinOutSet(LED0_port, LED0_pin);
        }
        if(values & COLLISION)
        {
            GPIO_PinOutSet(LED1_port, LED1_pin);
        }
        if(values & NOTOVER45)
            GPIO_PinOutClear(LED0_port, LED0_pin);
        if(values & NOTOVER75)
            GPIO_PinOutClear(LED1_port, LED0_pin);
        if(values & NOTCOLLISION)
            GPIO_PinOutClear(LED1_port, LED1_pin);
        /*   Check error code.                                  */
        APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
	}
}

/*	*********************************************************************************************************
 * 	
 * lcdDisplayTask()
 *@brief  
 *
 *@param
 *
 *@return		none
*********************************************************************************************************h
 * */
static void lcdDisplayTask(void * p_arg)
{
	RTOS_ERR err;
    DISPLAY_Init();

    /* Retarget stdio to a text display. */
    if (RETARGET_TextDisplayInit() != TEXTDISPLAY_EMSTATUS_OK) {
      while (1) ;
    }
    printf("\f");
    while(1)
    {
        printf("\f");
        OSMutexPend(&directionMutex,            			 	/*   Pointer to user-allocated mutex.         */
					0,                  				/*   Wait for a maximum of 1000 OS Ticks.     */
					OS_OPT_PEND_BLOCKING,  				/*   Task will block.                         */
					DEF_NULL,              				/*   Timestamp is not used.                   */
					&err);
        switch(dirInfo.dir)
        {
            case STRAIGHT:
                printf("Straight\n");
                break;
            case LEFT:
                printf("Left\n");
                break;
            case HARDLEFT:
                printf("Hard Left\n");
                break;
            case RIGHT:
                printf("Right\n");
                break;
            case HARDRIGHT:
                printf("Hard Right\n");
                break;
        }
        OSMutexPost(&directionMutex,         /*   Pointer to user-allocated mutex.         */
                OS_OPT_POST_NONE,     /*   Only wake up highest-priority task.      */
                &err);

        OSMutexPend(&speedMutex,            			 	/*   Pointer to user-allocated mutex.         */
            0,                  				/*   Wait for a maximum of 1000 OS Ticks.     */
            OS_OPT_PEND_BLOCKING,  				/*   Task will block.                         */
            DEF_NULL,              				/*   Timestamp is not used.                   */
            &err);
        printf("%d MPH\n", spdInfo.speed);
        OSMutexPost(&speedMutex,         /*   Pointer to user-allocated mutex.         */
					OS_OPT_POST_NONE,     /*   Only wake up highest-priority task.      */
					&err);
        OSTimeDly( 100,                                        /*   1000 OS Ticks                                      */
                OS_OPT_TIME_DLY,                             /*   from now.                                          */
                &err);
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
	OSSemPost (&timerSem,							/*   Pointer to user-allocated semaphore.     */
                OS_OPT_POST_1,					/*   Type of post     */
                &err);

    /*   Check error code.                                  */
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
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
    /* Initialize the display module. */

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

	OSSemCreate (&timerSem,								/*   Pointer to user-allocated semaphore.     */
	             "Capslide Semaphore",							/*   Name used for debugging.             */
	              0,											/*   initial value             */
	              &err);
    OSSemCreate (&queueSem,								/*   Pointer to user-allocated semaphore.     */
    			"Queue Semaphore",							/*   Name used for debugging.             */
				 0,											/*   initial value             */
				 &err);

    /*   Check error code.                                  */
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);



	/*   Check error code.                                  */
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

	/* Create the message queue.                           */
	OSQCreate(&messageQueue,           /*   Pointer to user-allocated message queue.          */
			  "Message Queue",          /*   Name used for debugging.                          */
			   20,                     /*   Queue will have 10 messages maximum.              */
			  &err);

	/*   Check error code.                                  */
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

    OSTaskCreate(&speedSetpointTaskTCB,                          /* Create the Button Task.                               */
                 "Speed Setpoint Task",
                  speedSetpointTask,
                  DEF_NULL,
                  EX_MAIN_START_TASK_PRIO,
                 &speedSetpointStk[0],
                 (EX_MAIN_START_TASK_STK_SIZE / 10u),
                  EX_MAIN_START_TASK_STK_SIZE,
                  0u,
                  0u,
                  DEF_NULL,
                 (OS_OPT_TASK_STK_CLR),
                 &err);
                                                                /*   Check error code.                                  */
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

    OSTaskCreate(&vehicleDirectionTaskTCB,                          /* Create the vehicle direction Task.                               */
                 "Vehicle Direction Task",
                  vehicleDirectionTask,
                  DEF_NULL,
                  EX_MAIN_START_TASK_PRIO,
                 &vehicleDirectionStk[0],
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

    OSTaskCreate(&vehicleMonitorTCB,                          /* Create the vehicle Monitor Task.                               */
                 "Vehicle Monitor Task",
                  vehicleMonitorTask,
                  DEF_NULL,
                  EX_MAIN_START_TASK_PRIO,
                 &vehicleMonitorStk[0],
                 (EX_MAIN_START_TASK_STK_SIZE / 10u),
                  EX_MAIN_START_TASK_STK_SIZE,
                  0u,
                  0u,
                  DEF_NULL,
                 (OS_OPT_TASK_STK_CLR),
                 &err);
                                                                /*   Check error code.                                  */
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

    OSTaskCreate(&lcdDisplayTCB,                          /* Create the LED Task.                               */
                "LCD Display Task",
                lcdDisplayTask,
                DEF_NULL,
                EX_MAIN_START_TASK_PRIO,
                &lcdDisplayStk[0],
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
    	OSTimeDly( 1000,                                        /*   1000 OS Ticks                                      */
                   OS_OPT_TIME_DLY,                             /*   from now.                                          */
                  &err);
                                                                /*   Check error code.                                  */
        APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
    }
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




  return 1;
}


