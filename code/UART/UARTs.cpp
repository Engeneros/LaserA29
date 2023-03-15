#include <UARTs.h>
#ifdef LPC2478
#include <lpc24xx_ex.h>
#endif
#ifdef LPC2148
#include <LPC2148_ex.h>
#endif
#include <uc.h>

using namespace Board;
using namespace uController;

Uart::Uart(unsigned char uN, unsigned char pG, unsigned char pN, unsigned long isrAddr, unsigned int rxBufSz, unsigned int nRxBuffs)
{		
	ffBuf = new TextRxCircle(rxBufSz, nRxBuffs);
	hw = usr = 0;
	txRd = 0;
	txWr =  0;
	txDel = 0;
	num = uN;
	uartIni(uN, true, true, isrAddr);
}

unsigned int Uart::GetMessageSize() const
{	
	return (usr == 0)? 0 : usr->GetSize();
}

void Uart::Update()
{
	unsigned int s = ffBuf->GetBufSz();
	while(s > 0)
	{
		Message* tmp = new Message(s);
		if(hw != 0)
			hw->SetNext(tmp);
		else
			usr = tmp;
		hw = tmp;
		ffBuf->GetMessage(hw->GetBuf());
		s = ffBuf->GetBufSz();
	}		
}


void Uart::GetMessage (char* d) 
{
	if(usr != 0)
	{
		usr->GetData(d);
		Message* tmp = usr;
		usr = usr->GetNext();
		if(usr == 0)
			hw = 0;
		delete tmp;
	}
}
	
void Uart::SendMessage(const char* buf)
{		
	TextMessageTx* tmp = new TextMessageTx();
	tmp->CopyMessage(buf);
	if (txWr == 0)
	{		
		txWr = tmp;				
	}
	else if (txRd != 0)
	{		
		txRd->SetNext(tmp);
	}
	txRd = tmp;
	if(UartTxIsReady(num) == true)
		TxByte();	
	while((txDel != 0) && (txDel != txWr))
	{
		tmp = txDel;
		txDel = txDel->GetNext();
		delete tmp;
	}
	txDel = 0;
}

void Uart::SendSymbol( char sym)
{
	char o[2]  = {0};	
	o[0]  = sym;
	SendMessage(o);
}

void Uart::ReadByte()
{
	ffBuf->AddSym(UartGet(num));
}

void Uart::TxByte()
{
	if( txWr != 0 )
	{	
		if(txWr->IsReady() == false)
		{			
			uartSend (num,  txWr->SendSym() );
		}
		else
		{	
			TextMessageTx* tmp = txWr;			
			txWr = txWr->GetNext();								
 			if(txWr != 0)								
 				uartSend (num,  txWr->SendSym() );			
			if(txDel == 0)
				txDel = tmp;				
		}
	}	
}

void Uart::IntEvent()
{	
	unsigned short iSrc = UartGetIntSrc(num);
 	if ( iSrc == 4 ) 
  		ReadByte();
	if ( iSrc == 2 )	
		TxByte();	
	VICVectAddr = 0;	
}

Uart::~Uart()
{
	UartDisableInt(num);
	delete  ffBuf;
	
	Message* tmp = hw;	
	while(usr != 0)
	{
		tmp = usr;
		delete tmp;
		usr = usr->GetNext();
	}	
	if(hw != 0)
		delete hw;
	

	while(txWr != 0)
	{
		txDel = txWr;
		delete txDel;
		txWr = txDel->GetNext();
	}	
	if(txRd != 0)
		delete txRd;
}
