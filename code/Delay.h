#ifndef DELAY_H
#define DELAY_H
namespace HwSleep
{ 
	extern void DelayMs(unsigned int delayInMs);
	extern void delay_ms(unsigned int delayInMs);
	extern void DelayUs(unsigned int delayInUs);
	extern void delay_us(unsigned int delayInUs);
	int SleepUs(unsigned int us);
	int SleepMs(unsigned int ms);
}

#endif



