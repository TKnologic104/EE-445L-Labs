// filename ******** fixed.c ************** 
#include <stdint.h>
#include "ST7735.c"
		int32_t xScreen = 128;
		int32_t yScreen = 160;
		int32_t border = 5;
		int32_t xOffset;
		int32_t yOffset;
		int32_t xScale;
		int32_t yScale;
		int32_t xStart;
		int32_t yStart;

/****************ST7735_sDecOut3***************
 converts fixed point number to LCD
 format signed 32-bit with resolution 0.001
 range -9.999 to +9.999
 Inputs:  signed 32-bit integer part of fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
 12345    " *.***"
  2345    " 2.345"  
 -8100    "-8.100"
  -102    "-0.102" 
    31    " 0.031" 
-12345    " *.***"
 */ 
void ST7735_sDecOut3(int32_t n){
  Messageindex = 0;
  fillmessage(n);
  Message[Messageindex] = 0; // terminate
  ST7735_DrawString(StX,StY,Message,StTextColor);
  StX = StX+Messageindex;
  if(StX>20){
    StX = 20;
    ST7735_DrawCharS(StX*6,StY*10,'*',ST7735_RED,ST7735_BLACK, 1);
  }


	int digs[4] = {0};
	int count = 0;
	if ((n <= 9999) && (n >= -9999)) {
		if (n >= 0){
			ST7735_OutChar(' ');
		}
		else {
			ST7735_OutChar('-');
			n = n * (-1);
		}
		while (n > 0){
			digs[count] = n%10;
			n = n / 10;
			count++;
		}
		count = 3;
		while (count >= 0){
			ST7735_OutChar(digs[count] + '0');
			if (count == 3){
				ST7735_OutChar('.');
			}
			count--;
		}
		
	}
	else {
		ST7735_OutChar(' ');
		ST7735_OutChar('*');
		ST7735_OutChar('.');
		ST7735_OutChar('*');
		ST7735_OutChar('*');
		ST7735_OutChar('*');
	}
//	StX = 0;
//	StY++;
//	if (StY > 20){
//		StY = 0;
//	}
}





/**************ST7735_uBinOut8***************
 unsigned 32-bit binary fixed-point with a resolution of 1/256. 
 The full-scale range is from 0 to 999.99. 
 If the integer part is larger than 256000, it signifies an error. 
 The ST7735_uBinOut8 function takes an unsigned 32-bit integer part 
 of the binary fixed-point number and outputs the fixed-point value on the LCD
 Inputs:  unsigned 32-bit integer part of binary fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
     0	  "  0.00"
     2	  "  0.01"
    64	  "  0.25"
   100	  "  0.39"
   500	  "  1.95"
   512	  "  2.00"
  5000	  " 19.53"
 30000	  "117.19"
255997	  "999.99"
256000	  "***.**"
*/
void ST7735_uBinOut8(uint32_t n){
  Messageindex = 0;
  fillmessage(n);
  Message[Messageindex] = 0; // terminate
  ST7735_DrawString(StX,StY,Message,StTextColor);
  StX = StX+Messageindex;
  if(StX>20){
    StX = 20;
    ST7735_DrawCharS(StX*6,StY*10,'*',ST7735_RED,ST7735_BLACK, 1);
  }
	return;
	char outArray[6] = {'*','*','*','.','*','*'};
	int fraction = 0;
	int intPart = 0;
	int count = 0;
	int bitValue = 0;
	if (n <= 256000) {
		while (count < 8){
//			fraction = fraction + (((n & (1 << count)) >> count ) / (2 ^ (8 - count)));
			bitValue = (n & (1 << count));
			if (bitValue > 0){
				fraction = fraction + (1000000 / (1 << (8 - count)));
			}
			count++;
		}
		fraction = fraction / 10000;
		outArray[5] = (fraction % 10) + '0';
		fraction = fraction / 10;
		outArray[4] = (fraction % 10) + '0';
		outArray[3] = '.';
		intPart = (n >> 8);
		outArray[2] = (intPart % 10) + '0';
		intPart = intPart / 10;
		outArray[1] = (intPart % 10) + '0';
		intPart = intPart / 10;
		outArray[0] = (intPart % 10) + '0';
	}
	ST7735_OutString(outArray);
}; 

/**************ST7735_XYplotInit***************
 Specify the X and Y axes for an x-y scatter plot
 Draw the title and clear the plot area
 Inputs:  title  ASCII string to label the plot, null-termination
          minX   smallest X data value allowed, resolution= 0.001
          maxX   largest X data value allowed, resolution= 0.001
          minY   smallest Y data value allowed, resolution= 0.001
          maxY   largest Y data value allowed, resolution= 0.001
 Outputs: none
 assumes minX < maxX, and miny < maxY
*/
void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY){
    ST7735_FillScreen(ST7735_BLACK); 
//  screen is actually 129 by 161 pixels, x 0 to 128, y goes from 0 to 160

		xOffset = 0 - minX;
		yOffset = 0 - maxY;
		uint32_t screenLen = 0;
		if (xScreen <= yScreen) {
			screenLen = xScreen;
		}
		else {
			screenLen = yScreen;
		}
		screenLen = screenLen - 2 * border;
		xScale = (maxX - minX)/(screenLen);
		yScale = (maxY - minY)/(screenLen);
		xStart = ((xScreen - (screenLen + 2 * border)) / 2 ) + border;
		yStart = ((yScreen - (screenLen + 2 * border)) / 2) + border;
	
    ST7735_SetCursor(1, 1);
	  printf(title);
	  ST7735_FillRect(xStart, yStart, screenLen, screenLen, ST7735_WHITE);  
};

/**************ST7735_XYplot***************
 Plot an array of (x,y) data
 Inputs:  num    number of data points in the two arrays
          bufX   array of 32-bit fixed-point data, resolution= 0.001
          bufY   array of 32-bit fixed-point data, resolution= 0.001
 Outputs: none
 assumes ST7735_XYplotInit has been previously called
 neglect any points outside the minX maxY minY maxY bounds
*/
void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[]){
	int32_t temp;
	for (int i = 0; i < num; i++){
	  ST7735_DrawPixel(xStart + (bufX[i] + xOffset)/ xScale, yStart - (bufY[i] + yOffset)/ yScale, ST7735_BLACK);
	}
};


