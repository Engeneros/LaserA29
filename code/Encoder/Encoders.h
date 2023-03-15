#ifndef ENCODERS_H
#define ENCODERS_H
#include <AbstractEncoder.h>
#include <uc.h>

using namespace uController;
namespace Board
{
	class Encoder : public AbstractEncoder
	{
	friend void EncGlobRouter(Encoder* en);	
	public:
		Encoder(Cap* a, Cap* b, Gpio* pa, Gpio* pb,  unsigned long isrAddr);	
		~Encoder();	
		virtual int GetAccumulatedValue();
		
	private:
		Cap* cA;
		Cap* cB;
		Gpio* pA;
		Gpio* pB;
		int value;
//		bool IsCapInt();
//		void SetCapInt();
//		void SetMatchInt();
	};

	void EncGlobRouter(Encoder* en) ;
}
#endif


