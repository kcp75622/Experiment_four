/*
 * @file main.c
 *
 * @brief Main source code for the PWM program.
 *
 * This file contains the main entry point and function definitions for the PWM program.
 * This lab demonstrates how to generate PWM signals using the two PWM modules: M0PWM and M1PWM.
 *
 * It interfaces with the following:
 *  - User LED (RGB) Tiva C Series TM4C123G LaunchPad
 *	- PMOD BTN module
 *	-	HS-485HB Servo
 *
 * It configures the pins used by the PMOD BTN module to generate interrupts on rising edges.
 * The PMOD BTN module will be used to update the duty cycle value for the HS-485HB Servo.
 * 
 * In addition, Timer 0A is used to generate periodic interrupts every 1 ms. It will
 * update the duty cycle for the RGB LED (specifically PF2) every 5 ms to demonstrate
 * a fading LED effect.
 *
 * @Katherine Poz
 */
#include "TM4C123GH6PM.h"
#include "SysTick_Delay.h"
#include "PWM_Clock.h"
#include "PWM1_3.h"

#include "PWM0_0.h"
#include "Timer_0A_Interrupt.h"
#include "PMOD_BTN_Interrupt.h"

// Declare the user-defined function prototype for Timer 0A
void Timer_0A_Periodic_Task(void);

// Declare the user-defined function prototype for PMOD_BTN_Interrupt
void PMOD_BTN_Handler(uint8_t pmod_btn_status);

// Check here
// Initialize a global variable for Timer 0A to keep track of elapsed time in milliseconds
static uint32_t Timer_0A_ms_elapsed = 0;

// Initialize a global variable to store the PWM duty cycle value for the RGB LED
static uint16_t RGB_LED_duty_cycle = 0;

// Initialize a global flag that determines if the PWM duty cycle for the RGB LED 
// shpuld be incremented or decremented
static uint8_t increment_duty_cycle = 1;
// End here

int main(void)
{
	// Initialize the SysTick timer used to provide blocking delay functions
	SysTick_Delay_Init();
	
	// Initialize the PWM clock to use the PWM clock divisor as the source
	// and update the PWM clock frequency to 50 MHz / 16 = 3.125 MHz
	PWM_Clock_Init();
	
	// Initialize the Module 1 PWM Generator 3 (PWM1_3) to generate a PWM signal
	// using the PF2 pin that has a frequency of 50 Hz with 5% duty cycle
	// Period constant = ((50 MHz /16) / 50 Hz) = 62500
	// Duty cycle = (62500 * 0.05) = 3125
	PWM1_3_Init(62500, 3125);
	
	//
	//
	//
	//
	PWM0_0_Init(62500, 1875);
	
	// Initialize the push buttons on the PMOD BTN module (Port A)
	PMOD_BTN_Interrupt_Init(&PMOD_BTN_Handler);
	
	// Initialize Timer A0 to generate periodic interrupts every 1 ms
	Timer_0A_Interrupt_Init(&Timer_0A_Periodic_Task);
	
	while(1)
	{		
		// Remove the lines in the while loop
	}
}

/**
* @brief Updates the duty cycle for the HS-485HB servo using the PMOD BTN module.
*
* @param uint8_t pmod_btn_status Indicates the status of the buttons on the PMOD BTN module.
*
* @return None
*/
void PMOD_BTN_Handler(uint8_t pmod_btn_status)
{
	switch (pmod_btn_status)
	{
		// BTN0 (PA2)
		case 0x04:
		{
			// 3% -> 600 us
			PWM0_0_Update_Duty_Cycle(1875);
			break;
		}
		
		// BTN1 (PA3)
		case 0x08:
		{
			// 11.5% -> 2.300 ms
			PWM0_0_Update_Duty_Cycle(3125);
			break;
		}
		
		// BTN2 (PA4)
		case 0x10:
		{
			PWM0_0_Update_Duty_Cycle(5000);
			break;
		}
		
		// BTN3 (PA5)
		case 0x20:
		{
			PWM0_0_Update_Duty_Cycle(7187);
			break;
		}
		
		default:
		{
			break;
		}
	}
}

/**
* @brief Updates the PWM duty cycle for the RGB LED every 5ms.
*
* The Timer_0A_Periodic_Task increments the PWM duty cycle for the RGB LED
* by 50 every 5ms. If the duty cycle is greated than or equal to 90%, then the
* RGB_LED_duty_cycle variable will reset to zero.
*
* @param None
*
* @return None
*/
void Timer_0A_Periodic_Task(void)
{
	Timer_0A_ms_elapsed++;
	
	if ((Timer_0A_ms_elapsed % 5) == 0)
	{
		RGB_LED_duty_cycle = RGB_LED_duty_cycle + 50;
		if (RGB_LED_duty_cycle >= 56250)
		{
			RGB_LED_duty_cycle = 0;
		}
		PWM1_3_Update_Duty_Cycle(RGB_LED_duty_cycle);
	}
	
	else
	{
		RGB_LED_duty_cycle = RGB_LED_duty_cycle - 50;
		
		if(RGB_LED_duty_cycle <= 0)
		{
			increment_duty_cycle = 1;
		}
	}
}
