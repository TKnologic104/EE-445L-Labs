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
 VALVANO SHALL NOT, IN ANY sCIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
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
#include "LCD.h"
#include "Switches.h"
#include "Speaker.h"
#include "Timer.h"
#include "ST7735.h"
#include "stdio.h"
#include "Title.h"
#include "math.h"

#define SCREEN_WIDTH 128
#define PF3                     (*((volatile uint32_t *)0x40025020))
#define PF2                     (*((volatile uint32_t *)0x40025010))
#define PF1                     (*((volatile uint32_t *)0x40025008))

/**** Function Declaration ****/

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void PortF_Init(void);

void ST7735_OutNum(char *ptr);



/****** Global Variables *******/

int main(void){
	
  PLL_Init(Bus80MHz);                   // 80 MHz
	PortF_Init();

	
	
	Timer0A_Init100HzInt();               // set up Timer0A for 100 Hz interrupts
	ResetScreenBlack();
	
	Timer1_Init();												// System Clock timer
	PortD_Init(); //Initialize Speaker
	PortF_Init(); //Initialize Switches
	

	while(1){
		DrawClockFace();
	}
}

void PortF_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
  while((SYSCTL_PRGPIO_R & 0x00000020) == 0){};
  GPIO_PORTF_DIR_R |= 0x0E;     // PF3,PF2,PF1 outputs
  GPIO_PORTF_DEN_R |= 0x0E;     // enable digital I/O on PF3,PF2,PF1  
}


