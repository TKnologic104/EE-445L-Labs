#include <stdint.h>
#include "ADCSWTrigger.h"
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "Timer1.h"
#include "Timer2.h"
#include "ST7735.h"
#include "stdio.h"
#include "Title.h"
#include "math.h"

#define RADIUS 30
#define OriginX 64
#define OriginY 96

void DrawSlantedLine(uint32_t numberOfLines);
void ST7735_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);
void DrawClockFace(void);
float minuteToRadian(uint32_t minute);

uint32_t XpolarToCoor(float theta);

uint32_t YpolarToCoor(float theta);

void DrawMinuteHand(uint32_t minute);


/******Global Variables******/


/**************Name: ResetScreenBlack***************
 Author: Karime Saad, Tarang Khandpur
 Description: Clears the screen to all Black.
 Inputs: none
 Outputs: none
*/
void ResetScreenBlack(void){
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_BLACK); 
  ST7735_SetCursor(0,0);
}
/**************Name: ResetScreenWhite***************
 Author: Karime Saad, Tarang Khandpur
 Description: Clears the screen to all White.
 Inputs: none
 Outputs: none
*/
void ResetScreenWhite(void){
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_WHITE); 
  ST7735_SetCursor(0,0);
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

/**************Name: DrawSlantedLine***************
 Author: Karime Saad, Tarang Khandpur
 Description: Draws lines at a 45 degree angle from
							the top left corner towards the bottom
							right corner of the LCD.
 Inputs: number of Lines you want to be drawn
 Outputs: none
*/
void DrawSlantedLine(uint32_t numberOfLines){
	ST7735_FillScreen(ST7735_WHITE);
	for (uint32_t i = 1; i < numberOfLines; i++){
		ST7735_Line (0,i*10,i*10,0, ST7735_BLUE);
	}
	DelayWait10ms(1000);
}
/********** Name: DrawClockFace ***********
 Author: Karime Saad, Tarang Khandpur
 Description: Draws a second hand clock to the
		
 Inputs: number of Lines you want to be drawn
 Outputs: none
*/
void DrawClockFace(void){
		ST7735_FillScreen(ST7735_WHITE);
		int32_t radius = 50;
		uint16_t xVal;
		uint16_t yVal;
		for (int i = 0; i < 60; i++){
			ST7735_FillScreen(ST7735_WHITE);
			float_t angle = (90 - (6 * i)) * 22/7 * 1.0/180; 
			float temp = (radius * 1000 * cos(angle) /1000);
			xVal = 64 + temp;
			temp = -1 * (radius * 1000 * sin(angle) /1000);
			yVal = 80 + temp;
			ST7735_Line (64,80,xVal,yVal, ST7735_BLUE);
		};
		ST7735_FillScreen(ST7735_WHITE);
}

void DrawMinuteHand(uint32_t minute){
	uint32_t Xcoor;
	uint32_t Ycoor;
	if (minute > 59){
		printf("error"); // temp error catch
	}
	Xcoor = XpolarToCoor(minuteToRadian(minute));
	Ycoor = YpolarToCoor(minuteToRadian(minute));
	//i need a draw line function which takes 2 points
	//what is each input?
	ST7735_Line (OriginX,OriginY,Xcoor,Ycoor, ST7735_BLUE);
}

float minuteToRadian(uint32_t minute){
	uint32_t degree;
	float radians;
	float theta;
	
	degree = minute*6;
	radians = ((float)degree)/(180.0*(22.0/7.0));
	theta = ((22.0/7.0)/2.0) - radians;
	
	return (theta);
}

uint32_t XpolarToCoor(float theta){
	float tempX = RADIUS*cos(theta);
	tempX += OriginX;
	return ((uint32_t)tempX);
}	

uint32_t YpolarToCoor(float theta){
	float tempY = RADIUS*sin(theta);
	tempY = OriginY - tempY;
	return ((uint32_t)tempY);
}	

