/********* main.c **************
 Author: Tarang Khandpur, Karime Saad
 Description: Main program to test Lab 3 Fall 2017
 Date: September 19, 2017
 
 Runs on TM4C123
 Uses ST7735.c LCD.
 
********************************/

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// center of X-ohm potentiometer connected to PE3/AIN0
// bottom of X-ohm potentiometer connected to ground
// top of X-ohm potentiometer connected to +3.3V 
#include <stdint.h>
#include "ADCSWTrigger.h"
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "Timer1.h"
#include "Timer2.h"
#include "ST7735.h"
#include "stdio.h"
#include "Title.h"
#include "math.h"

#define PF2   (*((volatile uint32_t *)0x40025010))
#define PF1   (*((volatile uint32_t *)0x40025008))
#define PF4   (*((volatile uint32_t *)0x40025040))
#define PMF_MAX_SIZE 4096
#define ARR_SIZE 1000
#define SCREEN_WIDTH 128

/**** Function Declaration ****/

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

void ResetScreenBlack(void);
void ResetScreenWhite(void);
void PortF_Init(void);
void Pause(void);
void DelayWait10ms(uint32_t n);
void ST7735_OutNum(char *ptr);

void Timer3A_Init10KHzInt(void);         
void SysTick_Init(void);       // initialize SysTick timer
void SysTick_Wait(uint32_t delay);
void SysTick_Disable(void);
void Timer3A_Disable(void);
void Timer0A_Init100HzInt(void);
void Timer0A_Handler(void);
void DrawSlantedLine(uint32_t numberOfLines);
void ST7735_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);
void DrawClockFace(void);


/****** Global Variables *******/



int main(void){
	
  PLL_Init(Bus80MHz);                   // 80 MHz
  SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
  ADC0_InitSWTriggerSeq3_Ch9();         // allow time to finish activating
	PortF_Init();	
	Timer1_Init();												// System Clock timer
	Timer0A_Init100HzInt();               // set up Timer0A for 100 Hz interrupts
	
	ResetScreenBlack();

	while(1){
	}
}


void PortF_Init(void){
	GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF

  PF2 = 0;                     				  // turn off LED
}



