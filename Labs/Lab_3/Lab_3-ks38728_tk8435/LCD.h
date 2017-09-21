#include <stdint.h>

/**************Name: ResetScreenBlack***************
 Author: Karime Saad, Tarang Khandpur
 Description: Clears the screen to all Black.
 Inputs: none
 Outputs: none
*/
void ResetScreenBlack(void);

/**************Name: ResetScreenWhite***************
 Author: Karime Saad, Tarang Khandpur
 Description: Clears the screen to all White.
 Inputs: none
 Outputs: none
*/
void ResetScreenWhite(void);

/*******Name: DelayWait10ms**********
// Description: Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: This function was provided to us
************************************/
void DelayWait10ms(uint32_t n);

/**************Name: DrawSlantedLine***************
 Author: Karime Saad, Tarang Khandpur
 Description: Draws lines at a 45 degree angle from
							the top left corner towards the bottom
							right corner of the LCD.
 Inputs: number of Lines you want to be drawn
 Outputs: none
*/
void DrawSlantedLine(uint32_t numberOfLines);

/********** Name: DrawClockFace ***********
 Author: Karime Saad, Tarang Khandpur
 Description: Draws a second hand clock to the
		
 Inputs: number of Lines you want to be drawn
 Outputs: none
*/
void DrawClockFace(void);

//****** DESCRIBE THESE BELOW *************//
float minuteToRadian(uint32_t minute);

uint32_t XpolarToCoor(uint32_t radius, float theta);

uint32_t YpolarToCoor(uint32_t radius, float theta);

void DrawMinuteHand(uint32_t minute);

void ResetScreenBackground(uint16_t BackgroundColor);

void DrawBackground(void);

void DrawHourHand(uint32_t hour, uint32_t minute);
