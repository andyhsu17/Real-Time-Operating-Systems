//***********************************************************************************
// Include files
//***********************************************************************************
#include "gpio.h"
#include "main.h"
#include "bspconfig.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************


//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
 * @brief Initializes GPIOs for polling and interrupts based on runtime define
 * @param  none
 * @return none
 *****************************************************************************/
void gpio_open(void){

	// Set LED ports to be standard output drive with default off (cleared)
//	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, LED0_default);

//	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
 	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, LED1_default);

/* Configure PB0 as input and enable interrupt  */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);

  /* Configure PB1 as input and enable interrupt */
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPull, 1);
#ifdef LAB2_USE_INTERRUPT
    GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, true, true, true);
  GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, true, true, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
#endif
}

/***************************************************************************//**
 * @brief GPIO funciton that ISRs call to unify the GPIO interrupts
 * @param  none
 * @return none
 *****************************************************************************/
void GPIO_Unified_IRQ(void)
{
  /* Get and clear all pending GPIO interrupts */
  uint32_t interruptMask = GPIO_IntGet();
  GPIO_IntClear(interruptMask);
  sample_button0();
  sample_button1();
  driveLEDs();
}

/***************************************************************************//**
 * @brief GPIO Interrupt handler for even pins
 ******************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  GPIO_Unified_IRQ();
}

/***************************************************************************//**
 * @brief GPIO Interrupt handler for odd pins
 ******************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  GPIO_Unified_IRQ();
}
