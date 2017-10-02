#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "stdio.h"
#include "Speaker.h"

//************* Definition Block **************
#define PF1                     (*((volatile uint32_t *)0x40025008))
#define PF2                     (*((volatile uint32_t *)0x40025010))
#define PF3                     (*((volatile uint32_t *)0x40025020))
#define PF4                     (*((volatile uint32_t *)0x40025040))
#define PD0                     (*((volatile uint32_t *)0x40007000))
#define PD1                     (*((volatile uint32_t *)0x40007008))
#define PD2                     (*((volatile uint32_t *)0x40007010))
#define PD3                     (*((volatile uint32_t *)0x40007020))
#define PD6											(*((volatile uint32_t *)0x400070D0))
#define PB1                     (*((volatile uint32_t *)0x40005008))

//************** Global Variable for Button ****************
enum Button {NONE = 0, UP = 1, DOWN = 2, BACK = 3, SET = 4, OFF = 5};
extern enum Button BtnPressed;
extern uint32_t IdleTime; //set this to 0 whenever a button is pressed;


//************** Function Declaration *****************
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void PortF_Init(void);
void PortD_Init(void);
void Switch_Init(void);
uint32_t GetButtonPressed(void);
void SwitchPB1_Init(void);
void Switch1_WaitPress(void);
void Switch2_WaitPress(void);
void Switch3_WaitPress(void);
void Switch4_WaitPress(void);
void Switch1_WaitRelease(void);
void Switch2_WaitRelease(void);
void Switch3_WaitRelease(void);
void Switch4_WaitRelease(void);


//******** Variables for Debouncing *******************
uint32_t buttons = 0;
volatile static unsigned long Touch1;     // true on touch button 1
volatile static unsigned long Release1;   // true on release button 1
volatile static unsigned long Touch2;     // true on touch button 2
volatile static unsigned long Release2;   // true on release button 2
volatile static unsigned long Touch3;     // true on touch button 3
volatile static unsigned long Release3;   // true on release button 3
volatile static unsigned long Touch4;     // true on touch button 4
volatile static unsigned long Release4;   // true on release button 4
volatile static unsigned long Touch5;     // true on touch button 5
volatile static unsigned long Release5;   // true on release button 5
volatile static unsigned long Last;      // previous

/**************Name: Timer0Arm***************
 Author: Karime Saad, Tarang Khandpur
 Description: One shot timer interrupt for 10ms
 Inputs: none
 Outputs: none
*/
static void Timer0Arm(void){
  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x0000001;    // 3) 1-SHOT mode
	TIMER0_TAILR_R = 220000;      // 4) 10ms reload value
  TIMER0_TAPR_R = 0;            // 5) bus clock resolution
  TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
  TIMER0_CTL_R = 0x00000001;    // 10) enable TIMER0A
}


/**************Name: GPIOArm***************
 Author: Karime Saad, Tarang Khandpur
 Description: Arm interrupt on PD(0-3)
 Inputs: none
 Outputs: none
*/
static void GPIOArm(void){
	GPIO_PORTB_ICR_R = 0xF;			//clear
	GPIO_PORTB_IM_R |= 0xF;
  GPIO_PORTD_ICR_R = 0xF;      // (e) clear flag4
  GPIO_PORTD_IM_R |= 0xF;      // (f) arm interrupt on PD0-PD3 *** No IME bit as mentioned in Book ***
  NVIC_PRI0_R = (NVIC_PRI0_R&0x00FFFFFF)|0xA0000000; // (g) priority 5
	NVIC_EN0_R |= 0x0000000A;      // (h) enable GPIO interrupt on Port D and Port B (IRQ bits)
}



/**************Name: Switch_Init***************
 Author: Karime Saad, Tarang Khandpur
 Description: Initialize switch interface, speaker, Port D and F
 Inputs: pointer to a function to call on touch (falling edge),
         pointer to a function to call on release (rising edge)
 Outputs: none
*/
void Switch_Init(void){
  // **** general initialization ****
  PortF_Init();
	PortD_Init();
	Speaker_Init();
  GPIOArm();
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
	
	Touch1 = 0;                   // touch and release signals
  Release1 = 0;
  Touch2 = 0;   
  Release2 = 0;
  Touch3 = 0;   
  Release3 = 0;
  Touch4 = 0;    
  Release4 = 0;
	Last = (PD3 | PD2 | PD1 | PD0);
	
 }
 
 
/**************Name: PortF_Init***************
 Author: Karime Saad, Tarang Khandpur
 Description: Initialize Port F
 Inputs: none
 Outputs: none
*/
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

/**************Name: PortD_Init***************
 Author: Karime Saad, Tarang Khandpur
 Description: Initialize Port D
 Inputs: none
 Outputs: none
*/
void PortD_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x08;        // 1) activate port D
  while((SYSCTL_PRGPIO_R&0x08)==0){};   // allow time for clock to stabilize
                                    // 2) no need to unlock PD3-0
//  GPIO_PORTD_AMSEL_R &= ~0x02;      // 3) disable analog functionality on PD3-0
	GPIO_PORTD_AMSEL_R = 0;       //     disable analog functionality on PD
  GPIO_PORTD_PCTL_R &= ~0x000000F0; // 4) GPIO
  GPIO_PORTD_DIR_R &= ~0x4F;         // 5) make PD0, PD1, PD2, PD3
  GPIO_PORTD_AFSEL_R &= ~0x4F;      // 6) regular port function
  GPIO_PORTD_DEN_R |= 0x4F;         // 7) enable digital I/O on PD3-0
	GPIO_PORTD_PUR_R |= 0x4F;     //     enable weak pull-up on PD3-0
	GPIO_PORTD_IS_R &= ~0x4F;     // (d) PF4 is edge-sensitive
}


/**************Name: GPIOPortD_Handler***************
 Author: Karime Saad, Tarang Khandpur
 Description: Performs debouncing on switch input
							Interrupt on rising or falling edge of PD0-3 (CCP0)
 Inputs: none
 Outputs: none
*/
void GPIOPortD_Handler(void){
  GPIO_PORTD_IM_R &= ~0x4F;     // disarm interrupt on PD0-3
	IdleTime = 0;

	if(Last & 0x08) {		// used for wait for button presses
		Touch4 = 1;
	} else {
		Release4 = 1;
	}
	if(Last & 0x4) {
		Touch3 = 1;
	} else {
		Release3 = 1;
	} 
	if(Last & 0x2) {
		Touch2 = 1;
	} else {
		Release2 = 1;
	}
	if(Last & 0x1) {
		Touch1 = 1;
	} else {
		Release1 = 1;
	}	
	
	if(GPIO_PORTD_RIS_R&0x01){   // page 213  --  PD0 was pressed
		BtnPressed = UP;
	} else if(GPIO_PORTD_RIS_R&0x02){   // page 213  --  PD1 was pressed
		BtnPressed = DOWN;
	} else if(GPIO_PORTD_RIS_R&0x04){   // page 213  --  PD2 was pressed
		BtnPressed = BACK;
	} else if(GPIO_PORTD_RIS_R&0x8){   // page 213  --  PD3 was pressed
		BtnPressed = SET;
	}
	Timer0Arm(); // start one shot
}

/**************Name: GPIOPortB_Handler***************
 Author: Karime Saad, Tarang Khandpur
 Description: Performs debouncing on switch input for port B
							and stops alarm.
 Inputs: none
 Outputs: none
*/
void GPIOPortB_Handler(void){
	GPIO_PORTB_IM_R &= ~0x2;     // disarm interrupt on PD0-3
	IdleTime = 0;
	if(GPIO_PORTB_RIS_R&0x02){   // page 213  --  PD6 was pressed
		PF1 ^= 0x01;
		BtnPressed = OFF;
		StopAlarm();
	}
  Timer0Arm(); // start one shot
}

/**************Name: Timer_0A_Handler***************
 Author: Karime Saad, Tarang Khandpur
 Description: Disarms interrupt.
 Inputs: none
 Outputs: none
*/
void Timer0A_Handler(void){
  TIMER0_IMR_R = 0x00000000;    // disarm timeout interrupt
	Last = (PD0 | PD1 | PD2 | PD3);  // switch state
  GPIOArm();   // start GPIO
}

uint32_t GetButtonPressed(void){
	return BtnPressed;
}

// Wait for switch to be pressed 
// There will be minimum time delay from touch to when this function returns
// Inputs:  none
// Outputs: none 


void Switch1_WaitPress(void){
	while(Touch1==0){}; // wait for press
		Touch1 = 0;  // set up for next time
}

void Switch2_WaitPress(void){
	while(Touch2==0){}; // wait for press
		Touch2 = 0;  // set up for next time
}

void Switch3_WaitPress(void){
	while(Touch3==0){}; // wait for press
		Touch3 = 0;  // set up for next time
}

void Switch4_WaitPress(void){
	while(Touch4==0){}; // wait for press
		Touch4 = 0;  // set up for next time
}


// Wait for switch to be released 
// There will be minimum time delay from release to when this function returns
// Inputs:  none
// Outputs: none 
void Switch1_WaitRelease(void){
  while(Release1==0){}; // wait
  Release1 = 0; // set up for next time
}
void Switch2_WaitRelease(void){
  while(Release2==0){}; // wait
  Release2 = 0; // set up for next time
}
void Switch3_WaitRelease(void){
  while(Release3==0){}; // wait
  Release3 = 0; // set up for next time
}
void Switch4_WaitRelease(void){
  while(Release4==0){}; // wait
  Release4 = 0; // set up for next time
}

// Return current value of the switch 
// Repeated calls to this function may bounce during touch and release events
// If you need to wait for the switch, use WaitPress or WaitRelease
// Inputs:  none
// Outputs: false if switch currently pressed, true if released 
unsigned long Switch_Input(void){
  return PF4;
}

