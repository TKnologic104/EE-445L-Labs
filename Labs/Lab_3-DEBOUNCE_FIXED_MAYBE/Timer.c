#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "Timer1.h"
#include "Timer2.h"
#include "ST7735.h"
#include "stdio.h"
#include "LCD.h"
<<<<<<< HEAD:Labs/Lab_3/Lab_3-ks38728_tk8435/Timer.c
=======
#include "Switches.h"
>>>>>>> 72a0e10a274e2e923d69801230e0b6af9000521a:Labs/Lab_3-DEBOUNCE_FIXED_MAYBE/Timer.c

//********* Function Declaration ****************
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void Timer3A_Init10KHzInt(void);  
void Timer2_Init(void);
void SysTick_Init(void);       // initialize SysTick timer
void SysTick_Wait(uint32_t delay);
void SysTick_Disable(void);
void Timer3A_Disable(void);
void Timer0A_Init100HzInt(void);
void Timer0A_Handler(void);
uint32_t checkMinute(void);


<<<<<<< HEAD:Labs/Lab_3/Lab_3-ks38728_tk8435/Timer.c

=======
>>>>>>> 72a0e10a274e2e923d69801230e0b6af9000521a:Labs/Lab_3-DEBOUNCE_FIXED_MAYBE/Timer.c
#define PF2   (*((volatile uint32_t *)0x40025010))
	
/******Global Variables******/
uint32_t ThirtySecCount;
extern uint32_t currentMinute;
extern uint32_t currentHour;
extern uint32_t timeChanged;
uint32_t IdleTime;

<<<<<<< HEAD:Labs/Lab_3/Lab_3-ks38728_tk8435/Timer.c

=======
/**************Name: Timer3A_Init10KHzInt***************
 Author: Karime Saad, Tarang Khandpur
 Description: Runs the heartbeat interrupt
 Inputs: none
 Outputs: none
*/
>>>>>>> 72a0e10a274e2e923d69801230e0b6af9000521a:Labs/Lab_3-DEBOUNCE_FIXED_MAYBE/Timer.c
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

  TIMER3_TAILR_R = 5000000;         // start value for 10,000 Hz interrupts; 80mhz / 10khz = 8000
	
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

/**************Name: Timer3A_Handler***************
 Author: Karime Saad, Tarang Khandpur
 Description: Runs the heartbeat interrupt
 Inputs: none
 Outputs: none
*/
void Timer3A_Handler(void){
	PF2 ^= 0x04;                   // heartbeat
  TIMER3_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer3A timeout
}

void Timer3A_Disable(void){
	TIMER3_CTL_R &= ~TIMER_CTL_TAEN;
}


/**************Name: SysTick_Init***************
 Author: Karime Saad, Tarang Khandpur
 Description: Initialize SysTick with busy wait running at bus clock.
 Inputs: none
 Outputs: none
*/

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


<<<<<<< HEAD:Labs/Lab_3/Lab_3-ks38728_tk8435/Timer.c
=======

/**************Name: Timer1A_Init30sec***************
 Author: Karime Saad, Tarang Khandpur
 Description: Initialize Timer1A 30 seconds
 Inputs: none
 Outputs: none
*/

>>>>>>> 72a0e10a274e2e923d69801230e0b6af9000521a:Labs/Lab_3-DEBOUNCE_FIXED_MAYBE/Timer.c
void Timer1A_Init30sec(void){
  volatile uint32_t delay;
  DisableInterrupts();
	PortF_Init();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x02;      // activate timer1
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER1_CTL_R &= ~TIMER_CTL_TAEN; // disable timer1A during setup
  TIMER1_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer1A initialization ****
                                   // configure for periodic mode
  TIMER1_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  
<<<<<<< HEAD:Labs/Lab_3/Lab_3-ks38728_tk8435/Timer.c
	//TIMER1_TAILR_R = 2400000000-1;         // start value for 30 sec interrupts
	TIMER1_TAILR_R = 999999;         // start value for testing interrupts
=======
	TIMER1_TAILR_R = 2400000000-1;         // start value for 30 sec interrupts
	//TIMER1_TAILR_R = 999999;         // start value for testing interrupts
>>>>>>> 72a0e10a274e2e923d69801230e0b6af9000521a:Labs/Lab_3-DEBOUNCE_FIXED_MAYBE/Timer.c
  
	TIMER1_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  TIMER1_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer1A=priority 2
	NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x00004000; // top 3 bits
  NVIC_EN0_R = 1<<21;              // enable interrupt 19 in NVIC
	
	ThirtySecCount = 0;
}

void Timer1A_Handler(void){
<<<<<<< HEAD:Labs/Lab_3/Lab_3-ks38728_tk8435/Timer.c
	
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
  PF2 ^= 0x04;                   // profile
  ThirtySecCount++;
	
	//Every time a button is hit, set IdleTime = 0;
=======
	
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
  ThirtySecCount++;
>>>>>>> 72a0e10a274e2e923d69801230e0b6af9000521a:Labs/Lab_3-DEBOUNCE_FIXED_MAYBE/Timer.c
	if (checkMinute()){
		timeChanged = 1;
		currentMinute++;
		IdleTime++;
		}
		if (currentMinute > 59){
			currentMinute = 0;
			currentHour++;
			if (currentHour > 24){
					currentHour = 0;
			}
		}
<<<<<<< HEAD:Labs/Lab_3/Lab_3-ks38728_tk8435/Timer.c
	PF2 ^= 0x04;                   // profile
}

uint32_t checkIdle(void){
	if (IdleTime >= 15){
=======
}

uint32_t checkIdle(void){
	if (IdleTime >= 2){
>>>>>>> 72a0e10a274e2e923d69801230e0b6af9000521a:Labs/Lab_3-DEBOUNCE_FIXED_MAYBE/Timer.c
		return 1;
	}
	return 0;
}

uint32_t checkMinute(void){
<<<<<<< HEAD:Labs/Lab_3/Lab_3-ks38728_tk8435/Timer.c
	
	//Testing on Simulator DELETE!
	//DelayWait10ms(1000);
	//ThirtySecCount++;
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^
=======
>>>>>>> 72a0e10a274e2e923d69801230e0b6af9000521a:Labs/Lab_3-DEBOUNCE_FIXED_MAYBE/Timer.c
	if (ThirtySecCount >= 2){
		ThirtySecCount = 0;
		return 1;
	}
	return 0;
<<<<<<< HEAD:Labs/Lab_3/Lab_3-ks38728_tk8435/Timer.c
=======
}


// ***************** Timer2_Init ****************
// Activate Timer2 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer2_Init(void){
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
	
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
	 //TIMER2_TAMR_R = TIMER_TAMR_TAMR_PERIOD
//	TIMER1_TAILR_R = 183954;  //reload for 1/440 Hz
//	TIMER1_TAILR_R = 90908;  //880Hz interrupts
	TIMER2_TAILR_R = 183954;  //440Hz Interrupts
//	TIMER1_TAILR_R =0x4C4B400;
//	TIMER1_TAILR_R = 39999;  
	TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x20000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
//  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
>>>>>>> 72a0e10a274e2e923d69801230e0b6af9000521a:Labs/Lab_3-DEBOUNCE_FIXED_MAYBE/Timer.c
}

