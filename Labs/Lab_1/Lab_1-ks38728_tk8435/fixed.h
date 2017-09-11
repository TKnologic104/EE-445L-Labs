/********* fixed.h **************
 Author: Tarang Khandpur, Karime Saad
 Description: C Header file for Lab 1 Fall 2017
 Date: September 10, 2017
*********************************/


/****************Name: ST7735_sDecOut3***************
 Author: Karime Saad, Tarang Khandpur
 Description: converts fixed point number to LCD
              format signed 32-bit with resolution 0.001
              range -9.999 to +9.999
 Inputs:  signed 32-bit integer part of fixed-point number
 Outputs: Prints exactly a 6 character fixed point number to LCD 
          Parameter LCD display
 12345    " *.***"
  2345    " 2.345"  
 -8100    "-8.100"
  -102    "-0.102" 
    31    " 0.031" 
-12345    " *.***"
 */ 
void ST7735_sDecOut3(int32_t n);


/**************Name: ST7735_uBinOut8***************
 Author: Karime Saad, Tarang Khandpur 
Description: unsigned 32-bit binary fixed-point with a resolution of 1/256. 
             The full-scale range is from 0 to 999.99. 
             If the integer part is larger than 256000, it signifies an error. 
             The ST7735_uBinOut8 function takes an unsigned 32-bit integer part 
             of the binary fixed-point number and outputs the fixed-point value on the LCD
 Inputs:  unsigned 32-bit integer part of binary fixed-point number
 Outputs: Prints exactly a 6 character fixed point number to LCD 
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
void ST7735_uBinOut8(uint32_t n); 

/**************Name: ST7735_XYplotInit***************
 Author: Karime Saad, Tarang Khandpur
 Description: Specify the X and Y axes for an x-y scatter plot
              Draw the title and clear the plot area
 Inputs:  title  ASCII string to label the plot, null-termination
          minX   smallest X data value allowed, resolution= 0.001
          maxX   largest X data value allowed, resolution= 0.001
          minY   smallest Y data value allowed, resolution= 0.001
          maxY   largest Y data value allowed, resolution= 0.001
 Outputs: Prints the Title for the shape to be drawn, and Sets the plot area to white background.
          assumes minX < maxX, and miny < maxY
*/
void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY);

/**************Name: ST7735_XYplot***************
 Author: Karime Saad, Tarang Khandpur
 Description: Plot an array of (x,y) data
 Inputs:  num    number of data points in the two arrays
          bufX   array of 32-bit fixed-point data, resolution= 0.001
          bufY   array of 32-bit fixed-point data, resolution= 0.001
 Outputs: Draws shape to cleared plot area
          assumes ST7735_XYplotInit has been previously called
          neglect any points outside the minX maxY minY maxY bounds
*/
void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[]);

void TitleScreen(char *labTitle, char *labDesc, char *st1FN, char *st1LN, char *st2FN, char *st2LN);

