void Timer3A_Init10KHzInt(void);

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void);

// This debug function initializes Timer0A to request interrupts
// at a 100 Hz frequency.  It is similar to FreqMeasure.c.
void Timer0A_Init100HzInt(void);

void Timer0A_Handler(void);
	
	
void (*PeriodicTask)(void);   // user function

// ***************** TIMER1_Init ****************
// Activate TIMER1 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer1_Init(void);

void Timer1A_Handler(void);
