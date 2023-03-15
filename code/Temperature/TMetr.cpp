#include <TMetr.h>
#include <LPC2148_ex.h>
 #include <Delay.h>

using namespace Board;
using namespace HwSleep;

Tmp05::Tmp05(unsigned  char pg, unsigned char pn, unsigned char nT, unsigned char nC,  unsigned long isrAddr)
{
	SleepMs(100);
	tS = new Cap(nT, nC, pg, pn, false, true, 1, 1, isrAddr,  true);
//	tS = new Cap(nT, nC, pg, pn, false, false, 1, 1, isrAddr,  true);
//	Cap(unsigned char nTim, unsigned char nCap, unsigned char pG, unsigned char pN,
//bool isFCap, bool isRCap, unsigned char pclkDiv, unsigned int presc, unsigned long isr, bool intEn)	
	if(nT == 0)
	{
		txIR = &T0IR;
		txCCR = &T0CCR;
		txCAP = &T0CR0  + nC;
	}	
	else if(nT == 1)
	{
		txIR = &T1IR;
		txCCR = &T1CCR;
		txCAP = &T1CR0 + nC;
	}	
	capInt = 0x10 << nC;
	isRise = 1 << (3 * nC);	
	tH = 1;
	tL = 1;	
}

unsigned long Tmp05::GetNeg()
{
	return tL;
}

unsigned long Tmp05::GetPos()
{
	return tH;
}

double Tmp05::GetT ()
{
	double ret, h = (double) tH, l = (double) tL;
 	if(tL > 0)
 	{
 		ret = 421.0 - (751.0 *   h) / l ;
 	}
	else
		ret = 0.0; 
 	return ret ;
//	return 0.0;
}



void Tmp05::IntEvent()
{	
	static unsigned int tPrev = 0;
	if(*txIR & capInt)  	//  capture iterrupt
	{
		unsigned int dt, tNow = *txCAP;
		if(tNow < tPrev)
			dt = tNow + (0xffffffff - tPrev);		
		else
			dt = tNow - tPrev;
		tPrev = tNow;
		
		if(*txCCR & isRise)  // rising front
		{
			tL = dt;
			tS->EnableOnlyFCap();
		}
		else
		{
			tH = dt;
			tS->EnableOnlyRCap();
		}
	}
	*txIR  = 0xff;	
	VICVectAddr = 0;
}

