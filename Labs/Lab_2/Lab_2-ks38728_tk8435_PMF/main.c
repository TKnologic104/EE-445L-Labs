/********* main.c **************
 Author: Tarang Khandpur, Karime Saad
 Description: Main program to test Lab 2 Fall 2017
 Date: September 17, 2017
 
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

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
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
void CalculateJitter(void);
void CalculatePMF(void);
void DrawPMF(void);
void ResetScreenBlack(void);
void ResetScreenWhite(void);
void PortF_Init(void);
void Pause(void);
void DelayWait10ms(uint32_t n);
void ST7735_OutNum(char *ptr);
void DisplayJitter(void);
void ClearPMF(void);
void Timer3A_Init10KHzInt(void);         
void SysTick_Init(void);       // initialize SysTick timer
void SysTick_Wait(uint32_t delay);
void SysTick_Disable(void);
void Timer3A_Disable(void);
void Timer0A_Init100HzInt(void);
void Timer0A_Handler(void);


/****** Global Variables *******/
volatile uint32_t ADCvalue;
uint32_t timeStamps[ARR_SIZE];
uint32_t adcValues[ARR_SIZE];
uint32_t currIndex = 0;
int32_t smallestTimeDiff = 0;
int32_t largestTimeDiff = 0;
uint32_t jitter = 0;
uint32_t pmf[PMF_MAX_SIZE];
int32_t pmfMinX = 0;
uint32_t pmfMaxX = 0;
uint32_t pmfMinY = 0;
uint32_t pmfMaxY = 0;
uint32_t pmfMidRangeX = 0;
uint32_t testCase = 1;
uint32_t testCount = 1;


int main(void){
  PLL_Init(Bus80MHz);                   // 80 MHz
  SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
  ADC0_InitSWTriggerSeq3_Ch9();         // allow time to finish activating
	PortF_Init();	
	Timer1_Init();												// System Clock timer
	Timer0A_Init100HzInt();               // set up Timer0A for 100 Hz interrupts
	
/******* Periodic Interrupts Init *******/
//	SysTick_Init();
//	SysTick_Wait(7999);
//	Timer3A_Init10KHzInt();
	
	while(1){

		EnableInterrupts();
				ResetScreenBlack();

		while(currIndex < ARR_SIZE){
			PF1 ^= 0x02;
			PF1 = (PF1*12345678)/1234567+0x02;  // this line causes jitter

		}
		CalculatePMF();
		DrawPMF();
		currIndex = 0;
//		CalculateJitter();
//		DisplayJitter();
//	  DelayWait10ms(2000);
//		currIndex = 0;
//		ResetScreenWhite();
//		CalculatePMF();
//		DrawPMF();
//	  DelayWait10ms(1000);
	}
}

void CalculateJitter(void){
	smallestTimeDiff = timeStamps[0] - timeStamps[1];
	largestTimeDiff = timeStamps[0] - timeStamps[1];
	int32_t delta = 0;

	for(uint32_t i=2; i<ARR_SIZE - 1; i++){
		delta = timeStamps[i - 1] - timeStamps[i];
		if(delta < smallestTimeDiff){
			smallestTimeDiff = delta;
		}
		if(delta > largestTimeDiff){
			largestTimeDiff = delta;
		}	
	}
	jitter = largestTimeDiff - smallestTimeDiff;
}

void DisplayJitter(void){
	ST7735_SetCursor(1,1);
	ST7735_OutString("jitter = ");
	ST7735_OutUDec(jitter);
}

void CalculatePMF(void){

//Add occurence of ADC value 
	pmfMinY = pmf[adcValues[0]];
	pmfMaxY = pmf[adcValues[0]];

	for(uint32_t i=0; i<ARR_SIZE; i++){
		pmf[adcValues[i]] += 1;

		if(pmf[adcValues[i]] < pmfMinY){
			pmfMinY = pmf[adcValues[i]];
		}
		if(pmf[adcValues[i]] > pmfMaxY){
			pmfMaxY = pmf[adcValues[i]];
			pmfMinX = adcValues[i] - SCREEN_WIDTH/2; // center value 
			if(pmfMinX > 4096){
				pmfMinX = 0;
			}
		}
	}
}

void DrawPMF(void){
		ST7735_PlotClear(pmfMinY, pmfMaxY);
		for(int i = pmfMinX; i < pmfMinX + SCREEN_WIDTH; i++){
		ST7735_PlotBar(pmf[i]); 
		ST7735_PlotNext();
	}
}

void ClearPMF(void){
	for(uint32_t i=0; i<PMF_MAX_SIZE; i++){
		pmf[i] = 0;
	}
}

void ResetScreenBlack(void){
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_BLACK); 
  ST7735_SetCursor(0,0);
}

void ResetScreenWhite(void){
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_WHITE); 
  ST7735_SetCursor(0,0);
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

/*******Name: Pause**********
// Description: Used to check if PF4, which is connected to switch 1, 
                has been pressed or not. 
// Inputs: None
// Outputs: None
// Notes: This function was provided to us.
************************************/
void Pause(void){
  while(PF4==0x00){ 
    DelayWait10ms(10);
  }
  while(PF4==0x10){
    DelayWait10ms(10);
  }
}
/*******Name: DelayWait10ms**********
// Description: Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: This function was provided to us
************************************/
void DelayWait10ms(uint32_t n){
	uint32_t volatile time;
  while(n){
    time = 727240*2/91;  // 10msec
    while(time){

	  	time--;
    }
    n--;
  }
}


/****** Periodic Interrupts *******/

void Timer3A_Init10KHzInt(void){
  volatile uint32_t delay;
  DisableInterrupts();
  // ** general initialization **
  SYSCTL_RCGCTIMER_R |= 0x08;      // activate timer3
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER3_CTL_R &= ~TIMER_CTL_TAEN; // disable timer3A during setup
  TIMER3_CFG_R = 0;                // configure for 32-bit timer mode
  // ** timer3A initialization **
  TIMER3_TAMR_R = TIMER_TAMR_TAMR_PERIOD; // configure for periodic mode, down count

  TIMER3_TAILR_R = 8000 - 3;         // start value for 10,000 Hz interrupts; 80mhz / 10khz = 8000
	
  TIMER3_TAPR_R = 0;         // bus clock resolution
  TIMER3_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER3_ICR_R = TIMER_ICR_TATOCINT;// clear timer3A timeout flag
  TIMER3_CTL_R |= TIMER_CTL_TAEN;  // enable timer3A 32-b, periodic

  // ** interrupt initialization **
// see table 9.1 of old book, 5.1 of new book
  NVIC_PRI8_R = (NVIC_PRI8_R&0x00FFFFFF)|0x20000000; // Timer3A=priority 1 top 3 bits
  NVIC_EN1_R = 1<<(35-32);  // enable interrupt 35 in NVIC. 
	//NVIC_EN0_R bit 31-0 control IRQ 31-0; NVIC_EN1_R bit 15-0 control ira 47-32
	// timer3a_handler has irq 35 and use 31-29 bits in NVIC_PRI8_R to control the priority
}
void Timer3A_Handler(void){
  TIMER3_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer3A timeout
}

void Timer3A_Disable(void){
	TIMER3_CTL_R &= ~TIMER_CTL_TAEN;
}



// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
//  NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_M;  // maximum reload value
  NVIC_ST_RELOAD_R = 7920;  // set period for 99 micro sec so 99 *10^-6 / 12.5 * 10^-9 =  reload value 7920
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x20000000; //priority 1 bits 31-29 , 0x4 will priority 2
  NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC+NVIC_ST_CTRL_INTEN; // enable SysTick with core clock
}

void SysTick_Disable(void){
	NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
}

void SysTick_Handler(void){
	int i = 0;
	i++;
}
// Time delay using busy wait.
// The delay parameter is in units of the core clock. (units of 20 nsec for 50 MHz clock)
void SysTick_Wait(uint32_t delay){
  volatile uint32_t elapsedTime;
  uint32_t startTime = NVIC_ST_CURRENT_R;
  do{
    elapsedTime = (startTime-NVIC_ST_CURRENT_R)&0x00FFFFFF;
  }
  while(elapsedTime <= delay);
}
// Time delay using busy wait.
// This assumes 50 MHz system clock.
void SysTick_Wait10ms(uint32_t delay){
  uint32_t i;
  for(i=0; i<delay; i++){
    SysTick_Wait(500000);  // wait 10ms (assumes 50 MHz clock)
  }
}

// This debug function initializes Timer0A to request interrupts
// at a 100 Hz frequency.  It is similar to FreqMeasure.c.
void Timer0A_Init100HzInt(void){
  volatile uint32_t delay;
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER0_TAILR_R = 799999;         // start value for 100 Hz interrupts
//	  TIMER0_TAILR_R = 799999/10;         // start value for 1000 Hz interrupts
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer0A=priority 2
	NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC
}

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
  PF2 ^= 0x04;                   // profile
  PF2 ^= 0x04;                   // profile
	
  ADCvalue = ADC0_InSeq3();
	PF2 ^= 0x04;                   // profile

	if(currIndex < ARR_SIZE){
		adcValues[currIndex] = ADCvalue;
		timeStamps[currIndex] = TIMER1_TAR_R;
		currIndex++;
	}
}
