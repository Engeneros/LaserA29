#ifndef UARTS_H
#define UARTS_H
#include <AbstractUart.h>
#include <ExchangeBuffers.h>
namespace Board
{ 
	class Uart : public AbstractUART
	{	
	public:			
		Uart( unsigned char uN, unsigned char pG, unsigned char pN, unsigned long isrAddrunsigned,
		unsigned int rxBufSz = MAX_RX_FIFO_SIZE, unsigned int nRxBuffs = MAX_RX_BUFFS );
		void IntEvent();
		virtual unsigned int GetMessageSize() const;
		virtual void Update();
		virtual void GetMessage  (char* d);
		virtual void SendMessage (const char* buf);
		virtual void SendSymbol( char sym);
		void ReadByte();
		void TxByte();
		virtual ~Uart();
	private:
		TextRxCircle* ffBuf;
		Message* hw;
		Message* usr;
		TextMessageTx* txRd;
		TextMessageTx* txWr;
		TextMessageTx* txDel;
		unsigned char num;
	};
}
#endif

