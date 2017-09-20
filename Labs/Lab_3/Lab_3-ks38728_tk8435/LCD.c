#include <stdint.h>
#include "ADCSWTrigger.h"
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "Timer1.h"
#include "Timer2.h"
#include "ST7735.h"
#include "stdio.h"
#include "Title.h"

void DrawSlantedLine(uint32_t numberOfLines);
void ST7735_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);
void DrawClockFace(void);

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



