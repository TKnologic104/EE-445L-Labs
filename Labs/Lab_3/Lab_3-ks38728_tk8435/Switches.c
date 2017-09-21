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
void Switch_Init(void);
uint32_t GetButtonPressed(void);

uint32_t buttonPressed = 0;

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


//Arm interrupt on PF4
static void GPIOArm(void){
  GPIO_PORTE_ICR_R = 0x1E;      // (e) clear flag4
  GPIO_PORTE_IM_R |= 0x1E;      // (f) arm interrupt on PE4, PE3, PE1, PE0 *** No IME bit as mentioned in Book ***
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
	NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
}

// Initialize switch interface on PF4 
// Inputs:  pointer to a function to call on touch (falling edge),
//          pointer to a function to call on release (rising edge)
// Outputs: none 
void Switch_Init(void){
  // **** general initialization ****
  PortF_Init();
  GPIOArm();
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
 }

 void PortF_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F
  while((SYSCTL_PRGPIO_R & 0x00000020) == 0){};
	GPIO_PORTF_DIR_R &= ~0x1E;    // (c) make PF4 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x1E;  //     disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x1E;     //     enable digital I/O on PF4   
  GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x1E;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x1E;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R |= 0x1E;     //     PF4 is both edges
}

// Interrupt on rising or falling edge of PF4 (CCP0)
void GPIOPortF_Handler(void){
  GPIO_PORTF_IM_R &= ~0x1E;     // disarm interrupt on PF4 
	if(GPIO_PORTF_RIS_R&0x02){   // page 213  --  PF1 was pressed
		buttonPressed = 1;
	}
	if(GPIO_PORTF_RIS_R&0x04){   // page 213  --  PF2 was pressed
		buttonPressed = 2;
	}
	if(GPIO_PORTF_RIS_R&0x08){   // page 213  --  PF3 was pressed
		buttonPressed = 3;
	}
	if(GPIO_PORTF_RIS_R&0x10){   // page 213  --  PF4 was pressed
		buttonPressed = 4;
	}
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
