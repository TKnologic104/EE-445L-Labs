// filename ******** fixed.c ************** 
// possible header file for Lab 1 Spring 2017
// feel free to change the specific syntax of your system
// Tarang Khandpur - Karime Saad
// 9/5/17


//imports
#include <stdio.h>
#include <stdint.h>
#include "string.h"
//#include "ST7735.h"
#include "PLL.h"
#include "float.h"
#include "inc/tm4c123gh6pm.h"


// version 1: C floating point
// run with compiler options selected for floating-point hardware

void Test1(void){
	volatile float T;    // temperature in C
	volatile uint32_t N; // 12-bit ADC value
	
  for(N=0; N<4096; N++){
    T = 10.0+ 0.009768*N; 	
  }
}


// version 2: C fixed-point
void Test2(void){
	volatile uint32_t T;    // temperature in 0.01 C
	volatile uint32_t N;    // 12-bit ADC value
  for(N=0; N<4096; N++){
    T = 1000 + (125*N+64)>>7; 	
  }
}

//__asm void
//	Test3(void){
//		  MOV R0,#0
//      LDR R1,=N    ;pointer to N
//      LDR R2,=T    ;pointer to T
//      VLDR.F32 S1,=0.009768   
//      VLDR.F32 S2,=10   
//loop3 STR R0,[R1]          ; N is volatile
//      VMOV.F32 S0,R0
//      VCVT.F32.U32 S0,S0   ; S0 has N
//      VMUL.F32 S0,S0,S1    ; N*0.09768
//      VADD.F32 S0,S0,S2    ; 10+N*0.0968
//      VSTR.F32 S0,[R2]     ; T=10+N*0.0968
//      ADD R0,R0,#1
//      CMP R0,#4096
//      BNE loop3
//      BX  LR
//		
//	}
	
