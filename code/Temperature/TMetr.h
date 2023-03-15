#ifndef T_METR_H
#define T_METR_H
#include <AbstractTmetr.h>
#include <uc.h>

using namespace uController;
namespace Board
{
	class Tmp05 : public AbstractTmetr
	{ 
	public:	
		Tmp05(unsigned  char pg, unsigned char pn, unsigned char nT, unsigned char nC,  unsigned long isrAddr);		
		virtual void IntEvent();
		virtual double GetT ();
		unsigned long GetNeg();
		unsigned long GetPos();
	private:
		Cap* tS;
		volatile unsigned long* txCCR;
		volatile unsigned long* txIR;
		volatile unsigned long* txCAP;
		unsigned long capInt, isRise;
		bool  isFirstRise, measure;
		unsigned long tH, tL, t0, t1, t2; 
		unsigned char stG, stN;
	};
}
#endif


