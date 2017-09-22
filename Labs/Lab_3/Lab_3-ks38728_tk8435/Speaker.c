//#include <stdint.h>
//#include "../inc/tm4c123gh6pm.h"

///*********PortD_Init***********
// Initializes Speaker on Port D*/

//#define PD1			(*((volatile uint32_t *) 0x40007008))

//void PortD_Init(void){
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

//void StartSpeaker(void){
//	PD1 ^= 0x01;
//}
