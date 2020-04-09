#include "em_device.h"
#include "em_chip.h"
#include <stdio.h>
#include <stdlib.h>
#include "buttons.h"
#include "capsense.h"
#include "slide.h"
#include "gpio.h"
#include "queue.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "cpu_cfg.h"
#include "os_cfg.h"
#include "shared_data.h"
#include "fifo.h"
#include "physics.h"
#include "main.h"

#include  <bsp_os.h>
#include  "bsp.h"
#include "display.h"
#include "textdisplay.h"
#include "glib.h"

#include  <cpu/include/cpu.h>
#include  <common/include/common.h>
#include  <kernel/include/os.h>
#include  <kernel/include/os_trace.h>

#include  <common/include/lib_def.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/toolchains.h>


#define LAB2_USE_INTERRUPT
#define LAB6_DONT_USE
#define FINAL_LAB_TEST
#define pylon_test_single
#define input_check


// state of pushbutton
uint8_t bstate0;
uint8_t bstate1;

uint8_t curr_direction; 
uint8_t prev_dir; //prev direction for timing
uint32_t left_cnt = 0; // count of left turns
uint32_t right_cnt = 0; // count of right turns
uint32_t dir_ms; // times hold of direction



static  CPU_STK  Ex_MainStartTaskStk[EX_MAIN_START_TASK_STK_SIZE];
static  OS_TCB   Ex_MainStartTaskTCB;

static  CPU_STK  AccelerationTaskStk[EX_MAIN_START_TASK_STK_SIZE];
static  OS_TCB   AccelerationTaskTCB;

static  CPU_STK  VehicleDirectionTaskStk[EX_MAIN_START_TASK_STK_SIZE];
static  OS_TCB   VehicleDirectionTaskTCB;

static  CPU_STK  LEDOutputTaskStk[EX_MAIN_START_TASK_STK_SIZE];
static  OS_TCB   LEDOutputInputTaskTCB;

static  CPU_STK  VehicleMonitorTaskStk[EX_MAIN_START_TASK_STK_SIZE];
static  OS_TCB   VehicleMonitorTaskTCB;

static  CPU_STK  LCDDisplayTaskStk[EX_MAIN_START_TASK_STK_SIZE];
static  OS_TCB   LCDDisplayTaskTCB;

static  CPU_STK  IdleTaskStk[EX_MAIN_START_TASK_STK_SIZE];
static  OS_TCB   IdleTaskTCB;

static  CPU_STK  RoadTaskStk[EX_MAIN_START_TASK_STK_SIZE];
static  OS_TCB   RoadTaskTCB;

static OS_SEM SpSetSem;
static OS_FLAG_GRP SpdDirFlag;

static OS_FLAG_GRP LEDFlag;

static OS_MUTEX SpdMutex;
static OS_MUTEX DirMutex;

InputFifo_t p_fifo;

VehicleSpeedData_t spdata;

VehicleDirectionData_t dirdata;

DisplayData_t disdata;


// OS timer callback function variable
static OS_TMR cap_timer;

// Boolean for callback timer
bool srt_dir = false;


/*
 * The functions below are the tasks that run the code
 * They all have parameters
 */
static void Ex_MainStartTask (void  *p_arg);
static void AccelerationTask (void  *p_arg);
static void VehicleDirectionTask (void  *p_arg);
static void VehicleMonitorTask (void *p_arg);
static void RoadMonitorTask (void *p_arg);
static void LCDDisplayTask (void *p_arg);
static void LedOutputTask (void  *p_arg);
static void IdleTask (void  *p_arg);
void tmr_callback_dir(void *p_tmr, void *p_arg);
void GPIO_EVEN_IRQHandler(void);
void GPIO_ODD_IRQHandler(void);


void tmr_callback_dir(void *p_tmr, void *p_arg)
{
	srt_dir = true;
}

/*
 * @brief: IRQ handler for even GPIO pins.
 * @param : none
 * @retval: none
 */
void GPIO_EVEN_IRQHandler(void)
{
	uint32_t flag = GPIO_IntGet();
	RTOS_ERR  err;
	GPIO_IntClear(flag);
	sampleButton0();
	sampleButton1();
	if(bstate0 == 1 && bstate1 == 0)
		InputFifo_Put(&p_fifo, 1);
	OSSemPost(&SpSetSem,
				OS_OPT_POST_ALL,
				&err);
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
}

/*
 * @brief: GPIO handler for all odd GPIOs
 * @param: none
 * return: none
 */
void GPIO_ODD_IRQHandler(void)
{
	uint32_t flag = GPIO_IntGet();
	RTOS_ERR  err;
	GPIO_IntClear(flag);
	sampleButton0();
	sampleButton1();
	if(bstate1 == 1 && bstate0 == 0)
		InputFifo_Put(&p_fifo, 2);
	OSSemPost(&SpSetSem,
				OS_OPT_POST_ALL,
					&err);
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
}





/*
 * task sets the speed based on button presses using ISR and a FIFO of commands
 * parameters: *p_arg
 * return: none
 */
static  void  AccelerationTask (void  *p_arg)
{

	RTOS_ERR  err;

	PP_UNUSED_PARAM(p_arg);                                     /* Prevent compiler warning.                            */

	float cur_spd = 0;
	uint8_t up_cnt = 0;
	uint8_t dwn_cnt = 0;

	InputValue_t p_value;

	CMU_ClockEnable(cmuClock_GPIO, true);

	GPIO_PinModeSet(BUTTONPORT, BUTTON0PIN, gpioModeInputPull, 1);

	GPIO_PinModeSet(BUTTONPORT, BUTTON1PIN, gpioModeInputPull, 1);

	GPIO_IntConfig(BUTTONPORT, BUTTON1PIN, true, true, true);
	GPIO_IntConfig(BUTTONPORT, BUTTON0PIN, true, true, true);

	NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);

	NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);

	while(1)
	{
        /* Acquire resource protected by semaphore.        */
		OSSemPend(&SpSetSem,        /* Pointer to user-allocated semaphore.    */
					0,                 /* Wait for a button presses to happpen    */
					OS_OPT_PEND_BLOCKING, /* Task will block.                        */
					DEF_NULL,             /* Timestamp is not used.                  */
				   &err);

		APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

		NVIC_DisableIRQ(GPIO_EVEN_IRQn);
		NVIC_DisableIRQ(GPIO_ODD_IRQn);

		while(p_fifo.head != p_fifo.tail)
		{
			if(InputFifo_Get(&p_fifo, &p_value))
			{
				if(p_value & 1)
				{
					cur_spd += 5;
					up_cnt  += 1;
				}
				else if(p_value & 2)
				{
					cur_spd -= 5;
					dwn_cnt  += 1;
				}
			}
		}

											/* Acquire resource protected by mutex.       */
		OSMutexPend(&SpdMutex,             /*   Pointer to user-allocated mutex.         */
					0,                  /*   Wait for a maximum of 1000 OS Ticks.     */
					OS_OPT_PEND_BLOCKING,  /*   Task will block.                         */
					DEF_NULL,              /*   Timestamp is not used.                   */
					&err);

		APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

		spdata.cur_speed += cur_spd;
		if(spdata.cur_speed < 0)
			spdata.cur_speed = 0;
		spdata.up_count += up_cnt;
		spdata.down_count += dwn_cnt;

									
		OSMutexPost(&SpdMutex,        
					OS_OPT_POST_NONE,    
					&err);

		APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

		cur_spd = 0;
		up_cnt = 0;
		dwn_cnt = 0;
		p_value = 0;

		OSFlagPost(&SpdDirFlag,             /*   Pointer to user-allocated event flag.    */
						SPD_CHG,            /*   Speed Flag bitmask.              */
						OS_OPT_POST_FLAG_SET,  /*   Set the flag.                            */
						&err);

		APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

		NVIC_EnableIRQ(GPIO_EVEN_IRQn);
		NVIC_EnableIRQ(GPIO_ODD_IRQn);



		OSTimeDly( 100,                                        /*   100 OS Ticks                                      */
		            OS_OPT_TIME_DLY,                             /*   from now.                                          */
		             &err);
		                                                                /*   Check error code.                                  */
		APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
	}

}

/*
 * task runs the cap_read function that determines whether the cap sensor was touched and where it was touched
 * 	to determine the direction the vehicle is turning
 * parameters: *p_arg
 * return: none
 */
static  void  VehicleDirectionTask (void  *p_arg)
{
	RTOS_ERR  err;

	PP_UNUSED_PARAM(p_arg);                                     /* Prevent compiler warning.                            */

	CAPSENSE_Init();

	while(1)
	{

		if(srt_dir)
		{
			srt_dir = false;
			prev_dir = dirdata.cur_direction;
			capSense();
												/* Acquire resource protected by mutex.       */
			OSMutexPend(&DirMutex,             /*   Pointer to user-allocated mutex.         */
						0,                  /*   Wait for Mutex Post.     */
						OS_OPT_PEND_BLOCKING,  /*   Task will block.                         */
						DEF_NULL,              /*   Timestamp is not used.                   */
						&err);

			APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

			dirdata.cur_direction = curr_direction;
			dirdata.cur_direction_ms = dir_ms;
			dirdata.left_count += left_cnt;
			dirdata.right_count += right_cnt;

												/* Release resource protected by mutex.       */
			OSMutexPost(&DirMutex,         		/*   Pointer to user-allocated mutex.         */
						OS_OPT_POST_NONE,     				/*   wake up all tasks.      */
						&err);

			APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

			curr_direction = 0;
			dir_ms = 0;
			left_cnt = 0;
			right_cnt = 0;

			/* Set Speed Flag .                    */
			OSFlagPost(&SpdDirFlag,             /*   Pointer to user-allocated event flag.    */
					   DIR_CHG,            /*   But 0 Flag bitmask.              */
					   OS_OPT_POST_FLAG_SET,  /*   Set the flag.                            */
					   &err);

			APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
			OSTimeDly( 100,                                        
					OS_OPT_TIME_DLY,                           
					&err);
			APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
		}
	}
}

/*
 * task monitors the speed and direction of vehicle to determine
 * 	unjust use of speed or turning and physics of the car
 * parameters: *p_arg
 * return: none
 */
static  void  VehicleMonitorTask (void *p_arg)
{
	RTOS_ERR  err;

	float cur_spd;
	int8_t dir;
	uint32_t time;
	//Vehicle_init();

	while(1)
	{

	    OSFlagPend(&SpdDirFlag,                /*   Pointer to user-allocated event flag. */
	    			SPD_CHG | DIR_CHG,             /*   Flag bitmask to match.                */
					100,                      /*   Wait for 100 OS Ticks maximum.        */
					OS_OPT_PEND_FLAG_SET_ANY    |/*   Wait until any flags are set and      */
					OS_OPT_PEND_BLOCKING     |/*    task will block and                  */
					OS_OPT_PEND_FLAG_CONSUME, /*    function will clear the flags.       */
					DEF_NULL,                 /*   Timestamp is not used.                */
				   &err);

	    OSMutexPend(&SpdMutex,            			 	/*   Pointer to user-allocated mutex.         */
					0,                  				/*   Wait for a maximum of 1000 OS Ticks.     */
					OS_OPT_PEND_BLOCKING,  				/*   Task will block.                         */
					DEF_NULL,              				/*   Timestamp is not used.                   */
					&err);
		/*   Check error code.                                  */
		APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

		cur_spd = spdata.cur_speed;

		/* Release resource protected by mutex.       */
		OSMutexPost(&SpdMutex,         /*   Pointer to user-allocated mutex.         */
					OS_OPT_POST_NONE,     /*   Only wake up highest-priority task.      */
					&err);

		/*   Check error code.                                  */
		APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);


        /* Acquire resource protected by mutex.       */
		OSMutexPend(&DirMutex,            			 	/*   Pointer to user-allocated mutex.         */
					0,                  				/*   Wait for a maximum of 1000 OS Ticks.     */
					OS_OPT_PEND_BLOCKING,  				/*   Task will block.                         */
					DEF_NULL,              				/*   Timestamp is not used.                   */
					&err);

		APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

		dir = dirdata.cur_direction;
		time = dirdata.cur_direction_ms;

		/* Release resource protected by mutex.       */
		OSMutexPost(&DirMutex,         /*   Pointer to user-allocated mutex.         */
					OS_OPT_POST_NONE,     /*   none.      */
					&err);

		/*   Check error code.                                  */
		APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);


		if((cur_spd >= 75) || (cur_spd >= 45 && (dir != STR8)))
		{
			OSFlagPost(&LEDFlag,             /*   Pointer to user-allocated event flag.    */
					   LED0_ON,            /*   But 0 Flag bitmask.              */
					   OS_OPT_POST_FLAG_SET,  /*   Set the flag.                            */
					   &err);

			APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
		}
		else
		{
			OSFlagPost(&LEDFlag,             /*   Pointer to user-allocated event flag.    */
					   LED0_OFF,            /*   But 0 Flag bitmask.              */
					   OS_OPT_POST_FLAG_SET,  /*   Set the flag.                            */
					   &err);

			APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
		}

		if(dir != STR8 && time > 5)
		{
			OSFlagPost(&LEDFlag,             /*   Pointer to user-allocated event flag.    */
					   LED1_ON,            /*   BTN 1 Flag bitmask.              */
					   OS_OPT_POST_FLAG_SET,  /*   Set the flag.                            */
					   &err);

			APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
		}
		else
		{
			OSFlagPost(&LEDFlag,             /*   Pointer to user-allocated event flag.    */
					   LED1_OFF,            /*   BTN 1 Flag bitmask.              */
					   OS_OPT_POST_FLAG_SET,  /*   Set the flag.                            */
					   &err);

			APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
		}


		OSTimeDly( 100,                                        /*   100 OS Ticks                                      */
					OS_OPT_TIME_DLY,                             /*   from now.                                          */
					 &err);
																		/*   Check error code.                                  */
		APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
	}
}

/*
 * @brief: A task that runs as a controller for the road and monitors activity of the car based on road conditions
 * parameters: *p_arg
 * return: none
 */
static void RoadMonitorTask (void  *p_arg)
{
	RTOS_ERR  err;
}

/*
 * task runs the LCD display to display Speed and Direction Information for User
 * parameters: *p_arg
 * return: none
 */
static void LCDDisplayTask (void *p_arg)
{
	RTOS_ERR  err;
	DISPLAY_Init();
	if (RETARGET_TextDisplayInit() != TEXTDISPLAY_EMSTATUS_OK) 
	{
	  while(1);
	}
	printf("Booting Up\nVehicle Info");
	/* Delay Task execution for                       */
	OSTimeDly( 2000,                                        /*   2000 OS Ticks                                      */
			OS_OPT_TIME_DLY,                             /*   from now.                                          */
			&err);
	/*   Check error code.                                  */
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
	printf("\f");
	while(1)
	{
		OSMutexPend(&SpdMutex,            			 	/*   Pointer to user-allocated mutex.         */
					0,                  				/*   Wait for a maximum of 1000 OS Ticks.     */
					OS_OPT_PEND_BLOCKING,  				/*   Task will block.                         */
					DEF_NULL,              				/*   Timestamp is not used.                   */
					&err);
        /*   Check error code.                                  */
		APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
		printf("\f");
		printf("Current Speed:\n %d MPH\n", (uint32_t)spdata.cur_speed);

										/* Release resource protected by mutex.       */
		OSMutexPost(&SpdMutex,         /*   Pointer to user-allocated mutex.         */
		OS_OPT_POST_NONE,     			/*   Only wake up highest-priority task.      */
		&err);

		APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);


		OSMutexPend(&DirMutex,            			 	/*   Pointer to user-allocated mutex.         */
					0,                  				/*   Wait for a maximum of 1000 OS Ticks.     */
					OS_OPT_PEND_BLOCKING,  				/*   Task will block.                         */
					DEF_NULL,              				/*   Timestamp is not used.                   */
					&err);
        /*   Check error code.                                  */
		APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

		if(dirdata.cur_direction == HARD_LEFT)
			printf("Direction: Hard Left\n");
		else if(dirdata.cur_direction == SOFT_LEFT)
			printf("Direction: Left\n");
		else if(dirdata.cur_direction == SOFT_RIGHT)
			printf("Direction: Right\n");
		else if(dirdata.cur_direction == HARD_RIGHT)
			printf("Direction: Hard Right\n");
		else
			printf("Direction: Straight\n");


		/* Release resource protected by mutex.       */
		OSMutexPost(&DirMutex,         /*   Pointer to user-allocated mutex.         */
					OS_OPT_POST_NONE,     			/*   Only wake up highest-priority task.      */
					&err);

		APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);


		OSTimeDly(150,                                        /*   100 OS Ticks                                      */
					OS_OPT_TIME_DLY,                             /*   from now.                                          */
					 &err);
																		/*   Check error code.                                  */
		APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
	}
}



/*
 * task runs the led_change function to change leds based on button task and slider task
 * parameters: *p_arg
 * return: none
 */
static  void  LedOutputTask (void  *p_arg)
{
	RTOS_ERR  err;

	OS_FLAGS led_flags;

	PP_UNUSED_PARAM(p_arg);                                     /* Prevent compiler warning.                            */

	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, LED0_default);

	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, LED1_default);

	while(1)
	{
		led_flags = OSFlagPend(&LEDFlag,                /*   Pointer to user-allocated event flag. */
								LED0_ON | LED1_ON | LED0_OFF | LED1_OFF,             /*   Flag bitmask to match.                */
								0,                      /*   Wait for 100 OS Ticks maximum.        */
								OS_OPT_PEND_FLAG_SET_ANY    |/*   Wait until any flags are set and      */
								OS_OPT_PEND_BLOCKING     |/*    task will block and                  */
								OS_OPT_PEND_FLAG_CONSUME, /*    function will clear the flags.       */
								DEF_NULL,                 /*   Timestamp is not used.                */
							    &err);

		APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

		if(led_flags & LED0_ON)
			led0_change(true);
		if(led_flags & LED0_OFF)
			led0_change(false);
		if(led_flags & LED1_ON)
			led1_change(true);
		if(led_flags & LED1_OFF)
			led1_change(false);



		OSTimeDly( 100,                                        /*   1000 OS Ticks                                      */
				OS_OPT_TIME_DLY,                             /*   from now.                                          */
				&err);
				                                                                /*   Check error code.                                  */
		APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

	}
}

/*
 * task idles when nothing else is running
 * parameters: *p_arg
 * return: none
 */
static void IdleTask (void *p_arg)
{
	PP_UNUSED_PARAM(p_arg);

	while(1)
	{
		EMU_EnterEM1();
	}
}
/*
 * task that creates the other tasks used in this lab - Acceleration, VehicleDirection, LEDOutput, and Idle
 * parameters: *p_arg
 * return: none
 */
static  void  Ex_MainStartTask (void  *p_arg)
{
    RTOS_ERR  err;


    PP_UNUSED_PARAM(p_arg);                                     /* Prevent compiler warning.                            */


    BSP_TickInit();                                             /* Initialize Kernel tick source.                       */


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

    // Create Periodic Timer for Vehicle Direction
    OSTmrCreate (&cap_timer,
                       "Cap Direction Timer",
                       0,
                       3,
					   OS_OPT_TMR_PERIODIC,
                       &tmr_callback_dir,
                       DEF_NULL,
                       &err);

    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

    // Create Semaphore
    OSSemCreate (&SpSetSem,
                       "Speed Set SEMAPHORE",
                       0,
                       &err);

    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

    // Create Speed Mutex
	OSMutexCreate(&SpdMutex,   		/*   Pointer to user-allocated mutex.                 */
					"Speed Mutex",  		/*   Name used for debugging.                         */
					&err);

	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

    // Create Event Flag Group for Speed Updates
    OSFlagCreate(&SpdDirFlag,              			/*   Pointer to user-allocated event flag.         */
                     "Speed/Direction Flags",             /*   Name used for debugging.                      */
                      0,                        /*   Initial flags, all cleared.                   */
                     &err);

    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

    // Create Speed Mutex
	OSMutexCreate(&DirMutex,   				/*   Pointer to user-allocated mutex.                 */
					"Direction Mutex",  		/*   Name used for debugging.                         */
					&err);

	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

    // Create Event Flag Group for Direction Updates
	OSFlagCreate(&LEDFlag,              			/*   Pointer to user-allocated event flag.         */
					 "LED Update Flags",             /*   Name used for debugging.                      */
					  0,                        /*   Initial flags, all cleared.                   */
					 &err);

    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

    // Create task for Speed Setpoint
	  OSTaskCreate(&AccelerationTaskTCB,                          /* Create the Start Task.                               */
						 "Speed Setpoint Task",
						  AccelerationTask,
						  DEF_NULL,
						  EX_MAIN_START_TASK_PRIO,
						 &AccelerationTaskStk[0],
						 (EX_MAIN_START_TASK_STK_SIZE / 10u),
						  EX_MAIN_START_TASK_STK_SIZE,
						  0u,
						  0u,
						  DEF_NULL,
						 (OS_OPT_TASK_STK_CLR),
						 &err);

      APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
      OSTaskCreate(&VehicleDirectionTaskTCB,                          /* Create the Start Task.                               */
                           "Vehicle Direction Task",
                            VehicleDirectionTask,
                            DEF_NULL,
                            EX_MAIN_START_TASK_PRIO,
                           &VehicleDirectionTaskStk[0],
                           (EX_MAIN_START_TASK_STK_SIZE / 10u),
                            EX_MAIN_START_TASK_STK_SIZE,
                            0u,
                            0u,
                            DEF_NULL,
                           (OS_OPT_TASK_STK_CLR),
                           &err);
	  OSTaskCreate(&VehicleMonitorTaskTCB,                          /* Create the Start Task.                               */
						 "Vehicle Monitor Task",
						 VehicleMonitorTask,
						  DEF_NULL,
						  EX_MAIN_START_TASK_PRIO,
						 &VehicleMonitorTaskStk[0],
						 (EX_MAIN_START_TASK_STK_SIZE / 10u),
						  EX_MAIN_START_TASK_STK_SIZE,
						  0u,
						  0u,
						  DEF_NULL,
						 (OS_OPT_TASK_STK_CLR),
						 &err);

	  OSTaskCreate(&RoadTaskTCB,                          /* Create the Start Task.                               */
						 "Road Monitor Task",
						 RoadMonitorTask,
						  DEF_NULL,
						  EX_MAIN_START_TASK_PRIO,
						 &RoadTaskStk[0],
						 (EX_MAIN_START_TASK_STK_SIZE / 10u),
						  EX_MAIN_START_TASK_STK_SIZE,
						  0u,
						  0u,
						  DEF_NULL,
						 (OS_OPT_TASK_STK_CLR),
						 &err);
      APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

	  OSTaskCreate(&LCDDisplayTaskTCB,                          /* Create the Start Task.                               */
					   "LCD Display Task",
					   LCDDisplayTask,
						DEF_NULL,
						EX_MAIN_START_TASK_PRIO,
					   &LCDDisplayTaskStk[0],
					   (EX_MAIN_START_TASK_STK_SIZE / 10u),
						EX_MAIN_START_TASK_STK_SIZE,
						0u,
						0u,
						DEF_NULL,
					   (OS_OPT_TASK_STK_CLR),
					   &err);

      /*   Check error code.                                  */
      APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

      // Create task for LED output
      OSTaskCreate(&LEDOutputInputTaskTCB,                          /* Create the Start Task.                               */
                           "LED Output Task",
                            LedOutputTask,
                            DEF_NULL,
                            EX_MAIN_START_TASK_PRIO,
                           &LEDOutputTaskStk[0],
                           (EX_MAIN_START_TASK_STK_SIZE / 10u),
                            EX_MAIN_START_TASK_STK_SIZE,
                            0u,
                            0u,
                            DEF_NULL,
                           (OS_OPT_TASK_STK_CLR),
                           &err);
      APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
      OSTaskCreate(&IdleTaskTCB,                          /* Create the Start Task.                               */
    		  "Idle Task",
			  IdleTask,
			  DEF_NULL,
			  IDLE_TASK_PRIO,
			  &IdleTaskStk[0],
			  (EX_MAIN_START_TASK_STK_SIZE / 10u),
			  EX_MAIN_START_TASK_STK_SIZE,
			  0u,
			  0u,
			  DEF_NULL,
			  (OS_OPT_TASK_STK_CLR),
			  &err);
            /*   Check error code.                                  */
      APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
      OSTmrStart (&cap_timer, &err);
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

int main(void)
{
  /* Chip errata */
  CHIP_Init();
  RTOS_ERR err;
  BSP_SystemInit();                                           /* Initialize System.                                   */
  CPU_Init();                                                 /* Initialize CPU.                                      */
  CPU_TS_TmrInit();
  OS_TRACE_INIT();
  OSInit(&err);                                               /* Initialize the Kernel.                               */

#ifdef FINAL_LAB_TEST

//void pylon_data(Waypoints prev, Waypoints curr, Waypoints* abspylon1, Waypoints* abspylon2);
Waypoints prev, curr, pylon1, pylon2;

char *name = "Spa";
int road[] = {
				110,10,
				130,20,
				137,40,
				150,90,
				174,87
			};
Road_init(name, road, 5);

#ifdef pylon_test_single

prev.x = 50;
prev.y = 10;

curr.x = 80;
curr.y = 5;

pylon_data(prev, curr, &pylon1, &pylon2);

#endif

#ifndef pylon_test_single
  uint16_t road[] = {
							  110,10,
							  130,20,
							  137,40,
							  150,90,
							  174,87
					  	  	};

  for(int i = 0; i < 8; i+=2)
  {
	  prev.x = road[i];
	  prev.y = road[i+1];
	  curr.x = road[i+2];
	  curr.y = road[i+3];
	  pylon_data(prev, curr, &pylon1, &pylon2);
  }
#endif

//   while(1);

#endif

#ifdef input_check



#endif


#ifndef FINAL_LAB_TEST

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

  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

                                                                  /*   Check error code.                                  */
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);


  OSStart(&err);                                              /* Start the kernel.                                    */

  /*   Check error code.                                  */
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

#ifndef LAB6_DONT_USE
  /* Infinite loop */
  while (1) {
#ifndef LAB2_USE_INTERRUPT
	  NVIC_DisableIRQ(GPIO_EVEN_IRQn);
	  NVIC_DisableIRQ(GPIO_ODD_IRQn);
	  capSense();
	  sampleButton0();
	  sampleButton1();

	  // Function to drive change in LEDs based on global variables
	  led_change();

	  // 100 ms delay for updates
	  for(int i = 0; i < 150000; i++);
#endif

#ifdef LAB2_USE_INTERRUPT
	  EMU_EnterEM1();
#endif
  }
#endif
#endif
}
