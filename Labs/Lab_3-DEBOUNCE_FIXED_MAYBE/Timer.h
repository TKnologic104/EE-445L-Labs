void Timer3A_Init10KHzInt(void);

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void);

// This debug function initializes Timer0A to request interrupts
// at a 100 Hz frequency.  It is similar to FreqMeasure.c.
void Timer1A_Init30sec(void);

void Timer1A_Handler(void);
		
uint32_t checkMinute(void);

uint32_t checkIdle(void);

void Timer2_Init(void);
