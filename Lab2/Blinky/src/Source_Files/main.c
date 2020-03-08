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
#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include "bsp.h"
#include "main.h"
#include "app.h"
#include "stdbool.h"
#include "capsense.h"

static volatile uint32_t msTicks; /* counts 1ms timeTicks */

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
 * @brief Delay function that delays based on SYSclk and counts number of ticks 
 * before reaching desired ticks
 * @param  dlyTicks number of clock cycles required to delay
 * @return none
 *****************************************************************************/
#ifndef LAB2_USE_INTERRUPT
static void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;
  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}
#endif
/***************************************************************************//**
 * @brief Systick IRQ handler increments ticks on desired interrupt
 * also calls slider functions when use interrupts is defined inside of main.h
 * @param  none
 * @return none
 *****************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;
#ifdef LAB2_USE_INTERRUPT
  CAPSENSE_Sense();
  sampleslider();
  driveLEDs();
#endif
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
   EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
   CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator and HFXO with kit specific parameters */
  /* Init DCDC regulator and HFXO with kit specific parameters */
  /* Initialize DCDC. Always start in low-noise mode. */
  // EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
   EMU_DCDCInit(&dcdcInit);
  // em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;
  // EMU_EM23Init(&em23Init);
   CMU_HFXOInit(&hfxoInit);

  /* Switch HFCLK to HFRCO and disable HFRCO */
   CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
   CMU_OscillatorEnable(cmuOsc_HFRCO, true, true);
  // CMU_OscillatorEnable(cmuOsc_HFXO, false, false);
  //  uint32_t clockspeed = SystemCoreClockGet();   // for debugging purposes
  
   if (SysTick_Config(SystemCoreClockGet() / 10)) {    // 40MHz clock divided by 10 gives 4,000,000 (number of ticks b/w each interrupt) gives 100ms
      while (1);
    }
  /* Call application program to open / initialize all required peripheral */
  app_peripheral_setup();
  CAPSENSE_Init();


  while (1)
  {
#ifndef LAB2_USE_INTERRUPT
    Delay(1);   // interrupt every 100ms
    sample_button0();
    sample_button1();
    CAPSENSE_Sense();
    sampleslider();
    driveLEDs();
#endif

#ifdef LAB2_USE_INTERRUPT
  EMU_EnterEM1();         // sleep
#endif
     
  }
}
