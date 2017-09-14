// ADCTestMain.c
// Runs on TM4C123
// This program periodically samples ADC channel 0 and stores the
// result to a global variable that can be accessed with the JTAG
// debugger and viewed with the variable watch feature.
// Daniel Valvano
// September 5, 2015

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
#include "ST7735.h"
#include "stdio.h"

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
#define PF4   (*((volatile uint32_t *)0x40025040))

#define PMF_MAX_SIZE 4096
#define ARR_SIZE 1000
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void CalculateJitter(void);
void CalculatePMF(void);
void CalculateXAxis(void);
void CalculateYAxis(void);
void DrawPMF(void);
void ResetScreen(void);
void PortF_Init(void);
void Pause(void);
void DelayWait10ms(uint32_t n);
void ST7735_OutNum(char *ptr);



/****** Global Variables *******/
volatile uint32_t ADCvalue;
uint32_t timeStamps[ARR_SIZE] = {0};
uint32_t adcValues[ARR_SIZE] = {0};
uint32_t currIndex = 0;
uint32_t jitter = 0;
uint32_t pmf[PMF_MAX_SIZE] = {0};
uint32_t pmfMinX = 0;
uint32_t pmfMaxX = 0;
uint32_t pmfMinY = 0;
uint32_t pmfMaxY = 0;

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
		timeStamps[currIndex] = TIMER1_TAR_R;
		adcValues[currIndex] = ADCvalue;
		currIndex ++;
	}
}
int main(void){
  PLL_Init(Bus80MHz);                   // 80 MHz
  SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
	
  ADC0_InitSWTriggerSeq3_Ch9();         // allow time to finish activating
  Timer0A_Init100HzInt();               // set up Timer0A for 100 Hz interrupts
	PortF_Init();	
	Timer1_Init();

  EnableInterrupts();
	
	while(1){
		
		while(currIndex < ARR_SIZE){
			PF1 ^= 0x02;  // toggles when running in main
		}
		
	DisableInterrupts();
		
		CalculateJitter();
		Pause();
//		CalculatePMF();
//		DrawPMF();
		
	}
}

void CalculateJitter(void){
	uint32_t smallestTimeDiff = timeStamps[0] - timeStamps[1];
	uint32_t largestTimeDiff = timeStamps[0] - timeStamps[1];
	uint32_t delta = 0;

	for(uint32_t i=1; i<ARR_SIZE - 1; i++){
		delta = timeStamps[i - 1] - timeStamps[i];
		if(delta < smallestTimeDiff){
			smallestTimeDiff = delta;
		}
		if(delta > largestTimeDiff){
			largestTimeDiff = delta;
		}	
	}
	jitter = largestTimeDiff - smallestTimeDiff;
	ResetScreen();
	char* interruptText = "One interrupt: "; 
	char* jitterText = "jitter =  ";
	ST7735_SetCursor(1,1);
	ST7735_DrawString(1, 2, interruptText, ST7735_WHITE);
	ST7735_DrawString(1, 3, jitterText, ST7735_WHITE);
	
	ST7735_SetCursor(10,3);

	char jitterArr[4] = {0};

		jitter = jitter / 10;
		jitterArr[2] = (jitter % 10) + '0';
		jitter = jitter / 10;
		jitterArr[1] = (jitter % 10) + '0';
		jitter = jitter / 10;
		jitterArr[0] = (jitter % 10) + '0';
	ST7735_OutString(jitterArr);
}

void CalculatePMF(void){
	CalculateXAxis();
	CalculateYAxis();
}

void CalculateXAxis(void){
	pmfMinX = adcValues[0];
	pmfMaxX = adcValues[0];
	for(uint32_t i=0; i<ARR_SIZE; i++){
		if(adcValues[i] < pmfMinX){
			pmfMinX = adcValues[i];
		}
		if(adcValues[i] > pmfMaxX){
			pmfMaxX = adcValues[i];
		}
	}
}

void CalculateYAxis(void){
	pmfMinY = pmf[adcValues[0]];
	pmfMaxY = pmf[adcValues[0]];

	for(uint32_t i=0; i<ARR_SIZE; i++){
		//Get range for Y axis: minY and maxY
		if(adcValues[i] < pmfMinY){
			pmfMinY = adcValues[i];
		}
		if(adcValues[i] > pmfMaxY){
			pmfMaxY = adcValues[i];
		}
		//Add occurence of ADC value 
		pmf[adcValues[i]] += 1;
	}
}

/**** NEEDS WORK *****/
void DrawPMF(void){
		ResetScreen();
	for(uint32_t x = pmfMinX; x<pmfMaxX; x++){
		if(adcValues[x] != 0){
			ST7735_PlotBar(pmf[adcValues[x]]);
//			ST7735_DrawFastVLine(adcValues[x], 20, pmf[adcValues[x]], ST7735_WHITE);
		}
		//ST7735_PlotNext();
	}
}

void ResetScreen(void){
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_BLACK); 
        ST7735_SetCursor(0,0);
}

void PortF_Init(void){
	GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
  PF2 = 0;                      // turn off LED
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

