#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Timer.h"

/*********PortD_Init***********
 Initializes Speaker on Port D*/

//#define PD1			(*((volatile uint32_t *) 0x40007008))

//void Speaker_Init(void){
//	
//  SYSCTL_RCGCGPIO_R |= 0x08;        // 1) activate port D
//  while((SYSCTL_PRGPIO_R&0x08)==0){};   // allow time for clock to stabilize
//                                    // 2) no need to unlock PD3-0
//  GPIO_PORTD_AMSEL_R &= ~0x02;      // 3) disable analog functionality on PD3-0
//  GPIO_PORTD_PCTL_R &= ~0x000000F0; // 4) GPIO
//  GPIO_PORTD_DIR_R |= 0x02;         // 5) make PD0
//  GPIO_PORTD_AFSEL_R &= ~0x02;      // 6) regular port function
//  GPIO_PORTD_DEN_R |= 0x02;         // 7) enable digital I/O on PD3-0

//}

#define PB1 (*((volatile uint32_t *)0x40005008))
//------------Switch_Init3------------
// Initialize GPIO Port B bit 1 for input
// Input: none
// Output: none
void Speaker_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x02;        // 1) activate clock for Port B
  while((SYSCTL_PRGPIO_R&0x02) == 0){};// ready?
  GPIO_PORTB_DIR_R |= 0x02;        // PB1 is an output
  GPIO_PORTB_AFSEL_R &= ~0x02;      // regular port function
  GPIO_PORTB_AMSEL_R &= ~0x02;      // disable analog on PB1 
  GPIO_PORTB_PCTL_R &= ~0x000000F0; // PCTL GPIO on PB1 
  GPIO_PORTB_DEN_R |= 0x02;         // PB3-0 enabled as a digital port
}


void StartAlarm(void){
	//Timer1 interrupt enable
	NVIC_EN0_R |= 1<<21;           // 9) enable IRQ 21 in NVIC
}

void StopAlarm(void){
	NVIC_EN0_R &= ~(1<<21);
}


//Should timeout every 440Hz or every 1/440 seconds
//Need to change this line in Timer1_Init() in Timer.c:   
//TIMER1_TAILR_R = 0xFFFFFFFF-1;    
//I changed it to this number, but needs verification
//TIMER1_TAILR_R = 0x3A2E8B2D; //I think this is 1/440 = 2.7272 milliseconds
//This will create a squarewave with a frequency of 440Hz which is A.

void Timer1A_Handler(void){
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
	while(1){
		PB1 ^= 0x02;
	}
}	
