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

uint32_t currentMinute;
uint32_t currentHour;
uint32_t alarmMinute;
uint32_t alarmHour;
uint32_t screenMinute;
uint32_t screenHour;
uint32_t cursorCol;
uint32_t cursorRow;
uint32_t alarmON;
uint32_t timeChanged;


enum State {MAIN_SCREEN = 0, OPTIONS_SCREEN = 1, SET_TIME_SCREEN = 2, SET_ALARM_SCREEN = 3, ON_OFF_ALARM_SCREEN = 4};
enum State CurrState;

enum Button {NONE = 0, UP = 1, DOWN = 2, BACK = 3, SET = 4, OFF = 5};
enum Button BtnPressed;
/**** Function Declaration ****/

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

void DrawScreen(uint32_t currentHour, uint32_t currentMinute);

int main(void){
	
  PLL_Init(Bus80MHz);                   // 80 MHz
  SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
	
	//Switch_Init();  //karime's code
	
	Timer1A_Init30sec();               // set up Timer1A for 30seconds

	//change speaker to timer 2
	InitScreen();

	while(1){
		EnableInterrupts();
		if (BtnPressed == NONE && CurrState == MAIN_SCREEN){ //BtnPrssed == None so that is continues to draw the main. when is isnt none is should continue to draw another screen
			if (timeChanged == 1){
				timeChanged = 0;
				DrawScreen(currentHour, currentMinute);
			}
		} 
		else {
			DrawScreen(currentHour, currentMinute);
			if (checkIdle()){ // this line says every 15mins the code will send you back to the main_screen
				//BtnPressed = SET;
				CurrState = MAIN_SCREEN;
				DrawScreen(currentHour, currentMinute);
			}
		}
	}
}

//TODO: Check Alarm - if (alarmON == 1 && currentHour == alarmHour && currentMinute == alaramMinute){
													// activite speaker

