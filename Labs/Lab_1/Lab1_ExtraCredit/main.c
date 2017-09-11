// Main.c
// Runs on LM4F120/TM4C123
// Use Timer2 in 32-bit periodic mode to request interrupts at a periodic rate
// Daniel Valvano
// May 5, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
  Program 7.5, example 7.6

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

// oscilloscope or LED connected to PF3-1 for period measurement
// When using the color wheel, the blue LED on PF2 is on for four
// consecutive interrupts then off for four consecutive interrupts.
// Blue is off for: dark, red, yellow, green
// Blue is on for: light blue, blue, purple, white
// Therefore, the frequency of the pulse measured on PF2 is 1/8 of
// the frequency of the Timer2A interrupts.
#include <stdint.h>
#include "PLL.h"
#include "Timer1.h"
#include "float.h"
#include "../inc/tm4c123gh6pm.h"


#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))

void checkTest1(void);
void checkTest2(void);
void checkTest3(void);
void checkTest4(void);
extern void Test3(void);
extern void Test4(void);

//uint32_t startTime1, startTime2, startTime3, startTime4; 
	uint32_t startTime;

uint32_t elapsedTime1, elapsedTime2, elapsedTime3, elapsedTime4; 



void PortF_Init(void){
	volatile uint32_t delay;
	SYSCTL_RCGCGPIO_R |= 0x20;       // activate port F
  delay = SYSCTL_RCGCGPIO_R;       // allow time to finish activating
  GPIO_PORTF_DIR_R |= 0x0E;        // make PF3-1 output (PF3-1 built-in LEDs)
  GPIO_PORTF_AFSEL_R &= ~0x0E;     // disable alt funct on PF3-1
  GPIO_PORTF_DEN_R |= 0x0E;        // enable digital I/O on PF3-1
                                   // configure PF3-1 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFF000F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;          // disable analog functionality on PF
}	

void checkTest1(void){
//  startTime1 = TIMER1_TAV_R;
	startTime = TIMER1_TAV_R;
	Test1();
	elapsedTime1 = (startTime - TIMER1_TAV_R);
}	
void checkTest2(void){
//  startTime2 = TIMER1_TAV_R;
		startTime = TIMER1_TAV_R;
	Test2();
	elapsedTime2 = (startTime - TIMER1_TAV_R);
}	
void checkTest3(void){
//  startTime3 = TIMER1_TAV_R;
		startTime = TIMER1_TAV_R;
	Test3();
	elapsedTime3 = (startTime - TIMER1_TAV_R);
}	
void checkTest4(void){
//	startTime4 = TIMER1_TAV_R;
		startTime = TIMER1_TAV_R;
	Test4();
	elapsedTime4 = (startTime - TIMER1_TAV_R);
}	


int main(void){ 
  PLL_Init(Bus80MHz);              // bus clock at 80 MHz
  PortF_Init();
	
  Timer1_Init(); // initialize timer1 (16 Hz)
	checkTest1();
	checkTest2();	
	checkTest3();
	checkTest4();
}

//Use this main function to check timing of tests through an oscilloscope
int main1(void){
  PortF_Init();
  while(1){
    //DelayWait10ms(100);
		PF2 ^= 0x04;
    PF2 ^= 0x04;
 }
}

