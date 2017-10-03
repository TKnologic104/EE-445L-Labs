<<<<<<< HEAD
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Timer.h"
#include "Switches.h"
#include "LCD.h"


#define PB1 (*((volatile uint32_t *)0x40005008))
#define PB5 (*((volatile uint32_t *)0x40005080))
#define PF2                     (*((volatile uint32_t *)0x40025010))
#define PF1                     (*((volatile uint32_t *)0x40025008))

	
uint32_t check = 0;
	

//------------Speaker_Init------------
// Initialize GPIO Port B bit 5 for output
// Input: none
// Output: none
void Speaker_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x02;        // 1) activate clock for Port B
  while((SYSCTL_PRGPIO_R&0x02) == 0){};// ready?
  GPIO_PORTB_DIR_R |= 0x20;        // PB5 is an output
	GPIO_PORTB_DIR_R &= ~0x02;				//PB1 is an input (Switch to turn off alarm)
//  GPIO_PORTB_AFSEL_R &= ~0x02;      // regular port function
//  GPIO_PORTB_AMSEL_R &= ~0x02;      // disable analog on PB1 
//	GPIO_PORTB_PCTL_R &= ~0x000000F0; // PCTL GPIO on PB1 
  GPIO_PORTB_DEN_R |= 0x22;         // PB5 enabled as a digital port
	GPIO_PORTB_PUR_R |= 0x02;     //     enable weak pull-up on PB1
	GPIO_PORTB_IS_R &= ~0x02;     // (d) PB1 is edge-sensitive
		
	PortF_Init();
}

void StartAlarm(void){
	//Timer1 interrupt enable
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
}

void StopAlarm(void){
	NVIC_DIS0_R = 1<<23;  
	PF1 = 0;
}

void Timer2A_Handler(void){
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER2A timeout
	PF1 ^= 0x02;
	PB5 ^= 0x20;
	check ++; //used for testing
}

=======
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Timer.h"
#include "Switches.h"
#include "LCD.h"


#define PB1 (*((volatile uint32_t *)0x40005008))
#define PB5 (*((volatile uint32_t *)0x40005080))
#define PF2                     (*((volatile uint32_t *)0x40025010))
#define PF1                     (*((volatile uint32_t *)0x40025008))

	
uint32_t check = 0;
	

//------------Speaker_Init------------
// Initialize GPIO Port B bit 5 for output
// Input: none
// Output: none
void Speaker_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x02;        // 1) activate clock for Port B
  while((SYSCTL_PRGPIO_R&0x02) == 0){};// ready?
  GPIO_PORTB_DIR_R |= 0x20;        // PB5 is an output
	GPIO_PORTB_DIR_R &= ~0x02;				//PB1 is an input (Switch to turn off alarm)
//  GPIO_PORTB_AFSEL_R &= ~0x02;      // regular port function
//  GPIO_PORTB_AMSEL_R &= ~0x02;      // disable analog on PB1 
//	GPIO_PORTB_PCTL_R &= ~0x000000F0; // PCTL GPIO on PB1 
  GPIO_PORTB_DEN_R |= 0x22;         // PB5 enabled as a digital port
	GPIO_PORTB_PUR_R |= 0x02;     //     enable weak pull-up on PB1
	GPIO_PORTB_IS_R &= ~0x02;     // (d) PB1 is edge-sensitive
		
	PortF_Init();
}

void StartAlarm(void){
	//Timer1 interrupt enable
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
}

void StopAlarm(void){
	NVIC_DIS0_R = 1<<23;  
	PF1 = 0;
}

void Timer2A_Handler(void){
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER2A timeout
	PF1 ^= 0x02;
	PB5 ^= 0x20;
	check ++; //used for testing
}

>>>>>>> Tarang
