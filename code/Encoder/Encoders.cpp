#include <Encoders.h>
#ifdef LPC2478
#include <LPC24xx.h>
#endif
#ifdef LPC2148
#include <LPC214x.h>
#endif

using namespace Board;

//unsigned long addrEnI;

//bool  IsCapInt()
//{
//	return  ((T0IR & 0xc0) == 0)? false : true;
//}

//void  Encoder::SetCapInt()
//{
//	
//}

//void   Encoder::SetMatchInt()
//{
//	IntervalGenerator(0, 600000,  (unsigned long) T0Match());
//}

//void Board::EncGlobRouter(Encoder* en) 
//{	
// 	static int dFi = 0;
//	static bool bState, aState, isA;
//	if(IsCapInt() == true)
//	{	
//		
//		en->cA->DisableCap();
//		en->cB->DisableCap();
//		if(en->cA->IsCap())   
//		{	
//			bState = en->pB->GetState();
//			isA = true;
//		}	
//		else if(en->cB->IsCap())   
//		{	
//			aState = en->pA->GetState();
//			isA = false;
//			dFi = (aState ^ bState)? 1 : -1;
//			en->value += dFi;					
//		}
////		IntervalGenerator(0, 600000,  (unsigned long) addrEnI);
////		TimerStart(0);
//	}
//	if(isA == true)
//	{
//	//	TimerStop(0);
////		en->cB->SetIsrAddr((unsigned long) addrEnI);
//		en->cB->EnableFRCap();
//		en->cA->ResetFlags();
//	}
//	else
//	{
////		TimerStop(0);
////		en->cA->SetIsrAddr((unsigned long) addrEnI);
//		en->cA->EnableFRCap();
//		en->cB->ResetFlags();
//	}
//	T0IR = 0xf0;
//	VICVectAddr = 0;
//}

void Board::EncGlobRouter(Encoder* en) 
{	
 	static int dFi = 0;
	static bool bState, aState;
	en->cA->DisableCap();
	en->cB->DisableCap();
	if(en->cA->IsCap())   
	{	
		bState = en->pB->GetState();
		en->cB->EnableFRCap();
		en->cA->ResetFlags();
	}	
	else if(en->cB->IsCap())   
	{	
		aState = en->pA->GetState();
		en->cA->EnableFRCap();
		en->cB->ResetFlags();
		dFi = (aState ^ bState)? 1 : -1;
		en->value += dFi;					
	}
	VICVectAddr = 0;
}

Encoder::Encoder(Cap* a, Cap* b, Gpio* pa, Gpio* pb,  unsigned long isrAddr)
{
	value = 0;
	cA = a;
	cB = b;
	pB = pb;
	pA = pa;
	cA->SetIsrAddr(isrAddr);
	cB->SetIsrAddr(isrAddr);
	cA->EnableFCap();
	cB->DisableCap();
	cA->EnableInt();
	cB->EnableInt();	
}
	
int Encoder::GetAccumulatedValue()
{
	int ret = value;
	value = 0;
	return ret;
}


Encoder::~Encoder()
{
	
}




