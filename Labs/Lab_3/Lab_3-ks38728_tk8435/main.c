/********* main.c **************
 Author: Tarang Khandpur, Karime Saad
 Description: Main program to test Lab 3 Fall 2017
 Date: September 19, 2017
 
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
 VALVANO SHALL NOT, IN ANY sCIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
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
#include "LCD.h"
#include "Switches.h"
#include "Speaker.h"
#include "Timer.h"
#include "ST7735.h"
#include "stdio.h"
#include "Title.h"
#include "math.h"

#define SCREEN_WIDTH 128
#define PF3                     (*((volatile uint32_t *)0x40025020))
#define PF2                     (*((volatile uint32_t *)0x40025010))
#define PF1                     (*((volatile uint32_t *)0x40025008))
#define MAIN_SCREEN 1
#define SET_TIME_SCREEN 2
#define SET_ALARM_SCREEN 3
#define OPTIONS_SCREEN 4
#define TRUE 1
#define FALSE 0

/**** Function Declaration ****/

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void ST7735_OutNum(char *ptr);

void DrawMainScreen(void);
void DrawOptionsScreen(void);
void DrawSetTimeScreen(void);
void DrawSetAlarmScreen(void);
void optionsUpArrow(void);
void optionsDownArrow(void);
void IncrementHour(void);
void DecrementHour(void);
void UpdateCurrentHour(void);
void IncrementAlarmHour(void);
void DecrementAlarmHour(void);
void UpdateAlarmTime(void);
void IncrementFirstMinute(void);
void IncrementSecondtMinute(void);
void DecrementFirstMinute(void);
void DecrementSecondtMinute(void);





/****** Global Variables *******/
uint32_t state = MAIN_SCREEN;
extern uint32_t buttonPressed;
uint32_t currentHour;
uint32_t currentMinute;
uint32_t alarmHour;
uint32_t alarmMinute;
uint32_t currentTime;
uint32_t alarmTime;
uint32_t option = 1;
uint32_t hourSet = FALSE;
uint32_t firstMinuteSet = FALSE;
uint32_t secondMinuteSet = FALSE;



int main(void){
  PLL_Init(Bus80MHz);                   // 80 MHz
	//PortD_Init(); //Initialize Speaker
	Switch_Init();
	Speaker_Init();
	Timer1_Init();

	
	DrawMainScreen();

	while(1){
		if(buttonPressed == 1){
			StartAlarm();
		}
		if(buttonPressed == 2){
			StopAlarm();
		}

//		switch(state){
//			case MAIN_SCREEN:
//				if(buttonPressed == 4){
//					state = OPTIONS_SCREEN;
//					DrawOptionsScreen();
//				}
//				break;
//			case OPTIONS_SCREEN:
//				switch(buttonPressed){
//					case 1:
//						option++;
//						if(option>3){
//							option = 1;
//						}
//						if(option<1){
//							option = 3;
//						}
//						optionsUpArrow();
//						break;
//					case 2:
//						option--;
//						if(option>3){
//							option = 1;
//						}
//						if(option<1){
//							option = 3;
//						}
//						optionsDownArrow();
//						break;
//					case 3:
//						state = MAIN_SCREEN;
//						DrawMainScreen();
//						break;
//					case 4: 
//						state = option;
//						if(state == 1){
//							DrawMainScreen();
//						}
//						if(state == 2){
//							DrawSetTimeScreen();
//						}
//						if(state == 3){
//							DrawSetAlarmScreen();
//						}
//						break;		
//					}
//				
//				if(buttonPressed == 1){
//					option++;
//					if(option>3){
//						option = 1;
//					}
//					if(option<1){
//						option = 3;
//					}
//					optionsUpArrow();
//				}
//				if(buttonPressed == 2){
//					option--;
//					if(option>3){
//						option = 1;
//					}
//					if(option<1){
//						option = 3;
//					}
//					optionsDownArrow();
//				}
//				if(buttonPressed == 3){
//					state = MAIN_SCREEN;
//					DrawMainScreen();
//				}
//				if(buttonPressed == 4){
//					state = option;
//					if(state == 1){
//						DrawMainScreen();
//					}
//					if(state == 2){
//						DrawSetTimeScreen();
//					}
//					if(state == 3){
//						DrawSetAlarmScreen();
//					}
//				}
//				break;
//			case SET_TIME_SCREEN:
//				switch(buttonPressed){
//					case 1:
//						if(hourSet != TRUE){
//							IncrementHour();
//						} else {
//							if(firstMinuteSet!=TRUE){
//								IncrementFirstMinute();
//							} else {
//								IncrementSecondMinute();
//							}
//						}
//						break;
//					case 2: 
//						if(hourSet != TRUE){
//							DecrementHour();
//						} else {
//							if(firstMinuteSet!=TRUE){
//								DecrementFirstMinute();
//							} else {
//								DecrementSecondMinute();
//							}
//						}
//						break;
//					case 3: 
//						state = OPTIONS_SCREEN;
//						DrawOptionsScreen();
//						break;
//					case 4:
//						if(hourSet != TRUE){
//							hourSet = TRUE;
//							UpdateCurrentHour();
//							MoveTimeArrow();
//						} else {
//							if(firstMinuteSet!= TRUE){
//								firstMinuteSet = TRUE;
//								UpdateFirstMinute();
//								MoveTimeArrow();
//							} else {
//								UpdateSecondMinute();
//								state = MAIN_SCREEN;
//								DrawMainScreen();
//							}
//						}
//						break;
//					}
//				break;
//			case SET_ALARM_SCREEN:
//				switch(buttonPressed){
//					case 1:
//						IncrementAlarmHour();
//						break;
//					case 2:
//						DecrementAlarmHour();
//						break;
//					case 3:
//						state = OPTIONS_SCREEN;
//						DrawOptionsScreen();
//						break;
//					case 4:
//						UpdateAlarmTime();
//						state = MAIN_SCREEN;
//						DrawMainScreen();
//						break;
//				}
//				break;
////			default:
////				state = MAIN_SCREEN;
////				DrawMainScreen();
//		}
	}
}

void DrawMainScreen(void){
	ResetScreenBlack();
	ST7735_OutString("MainScreen");
}
void DrawOptionsScreen(void){
	ResetScreenBlack();
	ST7735_OutString("OptionsScreen");
}
void DrawSetTimeScreen(void){
	ResetScreenBlack();
	ST7735_OutString("SetTimeScreen");
}
void DrawSetAlarmScreen(void){
	ResetScreenBlack();
	ST7735_OutString("SetAlarmScreen");
}

void optionsDownArrow(void){
	ST7735_OutString("option: ");
	ST7735_OutUDec(option);
}
void optionsUpArrow(void){
	ST7735_OutString("option: ");
	ST7735_OutUDec(option);
}
void IncrementHour(void){}
void DecrementHour(void){}
void UpdateCurrentHour(void){}
void IncrementFirstMinute(void){}
void IncrementSecondtMinute(void){}
void DecrementFirstMinute(void){}
void DecrementSecondtMinute(void){}
void IncrementAlarmHour(void){}
void DecrementAlarmHour(void){}
void UpdateAlarmTime(void){}

