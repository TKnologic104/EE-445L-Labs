// Switch.c
// Runs on TMC4C123
// Use GPIO in edge time mode to request interrupts on any
// edge of PF4 and start Timer0B. In Timer0B one-shot
// interrupts, record the state of the switch once it has stopped
// bouncing.
// Daniel Valvano
// May 3, 2015

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

// PF4 connected to a negative logic switch using internal pull-up (trigger on both edges)
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

//TK - test
#include "Speaker.h"
//
#define PF1                     (*((volatile uint32_t *)0x40025008))
#define PF2                     (*((volatile uint32_t *)0x40025010))
#define PF3                     (*((volatile uint32_t *)0x40025020))
#define PF4                     (*((volatile uint32_t *)0x40025040))
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void PortF_Init(void);
void PortD_Init(void);
void Switch_Init(void);
uint32_t GetButtonPressed(void);

uint32_t buttonPressed = 0;
uint32_t buttons = 0;

//One shot timer interrupt for 10ms
static void Timer0Arm(void){
  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x0000001;    // 3) 1-SHOT mode
  TIMER0_TAILR_R = 160000;      // 4) 10ms reload value
  TIMER0_TAPR_R = 0;            // 5) bus clock resolution
  TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
  TIMER0_CTL_R = 0x00000001;    // 10) enable TIMER0A
}


//Arm interrupt on PD(0-3)
static void GPIOArm(void){
  GPIO_PORTD_ICR_R = 0xF;      // (e) clear flag4
  GPIO_PORTD_IM_R |= 0xF;      // (f) arm interrupt on PD0-PD3 *** No IME bit as mentioned in Book ***
  NVIC_PRI0_R = (NVIC_PRI0_R&0x00FFFFFF)|0xA0000000; // (g) priority 5
	NVIC_EN0_R |= 0x00000008;      // (h) enable interrupt 30 in NVIC
}

// Initialize switch interface on PF4 
// Inputs:  pointer to a function to call on touch (falling edge),
//          pointer to a function to call on release (rising edge)
// Outputs: none 
void Switch_Init(void){
  // **** general initialization ****
  PortF_Init();
	PortD_Init();
  GPIOArm();
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
 }

//void PortF_Init(void){
//	SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F
//  while((SYSCTL_PRGPIO_R & 0x00000020) == 0){};
//	GPIO_PORTF_DIR_R &= ~0x1E;    // (c) make PF4 in (built-in button)
//  GPIO_PORTF_AFSEL_R &= ~0x1E;  //     disable alt funct on PF4
//  GPIO_PORTF_DEN_R |= 0x1E;     //     enable digital I/O on PF4   
//  GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
//  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
//  GPIO_PORTF_PUR_R |= 0x1E;     //     enable weak pull-up on PF4
//  GPIO_PORTF_IS_R &= ~0x1E;     // (d) PF4 is edge-sensitive
//  GPIO_PORTF_IBE_R |= 0x1E;     //     PF4 is both edges
//}

//// Interrupt on rising or falling edge of PF4 (CCP0)
//void GPIOPortF_Handler(void){
//  GPIO_PORTF_IM_R &= ~0x1E;     // disarm interrupt on PF4 
//	if(GPIO_PORTF_RIS_R&0x02){   // page 213  --  PF1 was pressed
//		buttonPressed = 1;
//	}
//	if(GPIO_PORTF_RIS_R&0x04){   // page 213  --  PF2 was pressed
//		buttonPressed = 2;
//	}
//	if(GPIO_PORTF_RIS_R&0x08){   // page 213  --  PF3 was pressed
//		buttonPressed = 3;
//	}
//	if(GPIO_PORTF_RIS_R&0x10){   // page 213  --  PF4 was pressed
//		buttonPressed = 4;
//		PF2 ^= 0x02;
//	}
//  Timer0Arm(); // start one shot
//}


void PortF_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F
  while((SYSCTL_PRGPIO_R & 0x00000020) == 0){};
	GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF

	PF1 = 0;
  PF2 = 0;                     				  // turn off LED
}
 
void PortD_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x08;        // 1) activate port D
  while((SYSCTL_PRGPIO_R&0x08)==0){};   // allow time for clock to stabilize
                                    // 2) no need to unlock PD3-0
//  GPIO_PORTD_AMSEL_R &= ~0x02;      // 3) disable analog functionality on PD3-0
	GPIO_PORTD_AMSEL_R = 0;       //     disable analog functionality on PD
  GPIO_PORTD_PCTL_R &= ~0x000000F0; // 4) GPIO
  GPIO_PORTD_DIR_R &= ~0xF;         // 5) make PD0, PD1, PD2, PD3
  GPIO_PORTD_AFSEL_R &= ~0xF;      // 6) regular port function
  GPIO_PORTD_DEN_R |= 0xF;         // 7) enable digital I/O on PD3-0
	GPIO_PORTD_PUR_R |= 0xF;     //     enable weak pull-up on PD3-0
	GPIO_PORTD_IS_R &= ~0xF;     // (d) PF4 is edge-sensitive
}

// Interrupt on rising or falling edge of PD0-3 (CCP0)
void GPIOPortD_Handler(void){
  GPIO_PORTD_IM_R &= ~0xF;     // disarm interrupt on PD0-3
//	PF2 = 0x04;
//	
//	/****try 0*****/
	if(GPIO_PORTD_RIS_R&0x01){   // page 213  --  PD0 was pressed
		PF2 ^= 0x04;
		buttonPressed = 1;
		StartAlarm();
	} 
	if(GPIO_PORTD_RIS_R&0x02){   // page 213  --  PD1 was pressed
		PF2 ^= 0x04;
		buttonPressed = 2;
		StopAlarm();
	}
	if(GPIO_PORTD_RIS_R&0x04){   // page 213  --  PF3 was pressed
		PF2 ^= 0x04;
		buttonPressed = 3;
	}
	if(GPIO_PORTD_RIS_R&0x8){   // page 213  --  PF4 was pressed
		PF2 ^= 0x04;
		buttonPressed = 4;
	}

/****try 1***/
//buttons = GPIO_PORTD_DATA_R&0xF;
//	switch(buttons) {

//   case 0x1  :
//		 	PF2 = 0x04;
//      buttonPressed = 1;
//      break; 
//   case 0x02  :
//			PF1 = 0x02;
//      buttonPressed = 2;
//      break; 
//	 case 0x4  :
//      buttonPressed = 3;
//      break; 
//   case 0x08  :
//      buttonPressed = 4;
//      break;
//   default : 
//   buttonPressed = buttonPressed;
//}

/***try 2****/
//	buttonPressed = GPIO_PORTD_DATA_R&0xF;
	
	
  Timer0Arm(); // start one shot
}


 
// Interrupt 10 ms after rising edge of PF4
void Timer0A_Handler(void){
  TIMER0_IMR_R = 0x00000000;    // disarm timeout interrupt
  GPIOArm();   // start GPIO
}

uint32_t GetButtonPressed(void){
	return buttonPressed;
}

// Wait for switch to be pressed 
// There will be minimum time delay from touch to when this function returns
// Inputs:  none
// Outputs: none 
void Switch_WaitPress(void){
//  while(Touch==0){}; // wait for press
//  Touch = 0;  // set up for next time
}

// Wait for switch to be released 
// There will be minimum time delay from release to when this function returns
// Inputs:  none
// Outputs: none 
void Switch_WaitRelease(void){
//  while(Release==0){}; // wait
//  Release = 0; // set up for next time
}

// Return current value of the switch 
// Repeated calls to this function may bounce during touch and release events
// If you need to wait for the switch, use WaitPress or WaitRelease
// Inputs:  none
// Outputs: false if switch currently pressed, true if released 
unsigned long Switch_Input(void){
  return PF4;
}

