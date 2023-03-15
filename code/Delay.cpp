#ifdef LPC2478
#include <lpc24xx_ex.h>
#endif
#ifdef LPC2148
#include <LPC2148_ex.h>
#endif
#include <Delay.h>
using namespace HwSleep;

void HwSleep::DelayMs(unsigned int delayInMs)
{
#ifdef LPC2478
	PCLKSEL0	&= ~(3 << 2);
	PCLKSEL0 |= 1 << 2;
#endif	
  T0TCR = 0x02; // Stop and reset timer.
  T0PR  = 0x00; // Set prescaler to zero.
  T0MR0 = delayInMs * (60000000/ 1000); // Set Value.
  T0IR  = 0xff; // Reset all interrrupt flags.
  T0MCR = 0x04; // Stop timer on match.
  T0TCR = 0x01; // Start timer.
  while (T0TCR & 0x01);
}

void HwSleep::DelayUs(unsigned int delayInUs)
{
#ifdef LPC2478	
	PCLKSEL0	&= ~(3 << 2);
	PCLKSEL0 |= 1 << 2;
#endif	
  T0TCR = 0x02; // Stop and reset timer.
  T0PR  = 0x00; // Set prescaler to zero.
  T0MR0 = delayInUs * (60000000/ 1000000); // Set Value.
  T0IR  = 0xff; // Reset all interrrupt flags.
  T0MCR = 0x04; // Stop timer on match.
  T0TCR = 0x01; // Start timer.
  while (T0TCR & 0x01);
}

int HwSleep::SleepUs(unsigned int us)
{
	int a = 0;
	for(int i = 5 * us; i > 0; i--)
		a += i; 
	return a;
}


int HwSleep::SleepMs(unsigned int ms)
{
	int a = 0;
	for(int i = ms; i > 0; i--)
		a += SleepUs(1000); 
	return a;		
}


void HwSleep::delay_us(unsigned int delayInUs)
{
 	DelayUs(delayInUs);
}

void HwSleep::delay_ms(unsigned int delayInMs)
{
 	DelayMs(delayInMs);
}



