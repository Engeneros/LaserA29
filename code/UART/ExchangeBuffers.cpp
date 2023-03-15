#include <ExchangeBuffers.h>
#include <stdlib.h>

using namespace Board;

/////////////////////============Message=============///////////////////////
Message::Message(unsigned int sz)
{
	SetSize(sz);
	nxt = 0;
}

void Message::SetNext(Message* m)
{
	nxt = m;
}


Message* Message::GetNext()
{
	return nxt;	
}


unsigned int Message::GetSize() const
{
	return size;
}

void Message::SetSize(unsigned int s)
{
	size = s;
	if(size > 0)
		buf = new char[size];
	else
		buf = 0;
}

char* Message::GetBuf()
{
	return buf; 
}

void Message::GetData(char* d)		
{
	for(int i = 0; i < size; ++i)
		d[i] = buf[i];
}

void Message::SetData(const char* d)
{
	for(int i = 0; i < size; ++i)
		buf[i] = d[i];
}

Message::~Message()
{
	delete [] buf;
}

/////////////////////============TextRxCircle=============///////////////////////
TextRxCircle::TextRxCircle(unsigned int sz, unsigned int nBuffs)
{
	size = sz;
	nB = nBuffs;
	bAddr = cAddr = 0;
	fifo = new char [size];
	hwWr = new RxBuffInfo;
	hwWr->ready = false;
	hwWr->sz = 0;			 		
	usrRd = hwWr;
	RxBuffInfo* first = hwWr;
	for(int i = 1; i < nB ; ++i)
	{		
		hwWr = new RxBuffInfo;
		hwWr->ready = false;
		hwWr->sz = 0;		

		usrRd->next = hwWr;
		usrRd = hwWr;
	}
	usrRd->next = first;
	
}
unsigned int TextRxCircle::GetBufSz() const
{
	return  (usrRd->ready == false)? 0 : usrRd->sz;		
}

void TextRxCircle::AddSym(char symb)
{					/// =====!!! interrupt inside work======
	
	if ( symb <= END_RX2)// || (symb == 0xA)   ) 
	{
		if ( hwWr->sz > 0 )
		{					
			fifo[cAddr++] = 0;
			cAddr %= size;
			hwWr->sz++;	
			
			hwWr->bgAddr = bAddr;
			bAddr = cAddr;
			hwWr->ready = true;
			hwWr = hwWr->next;
			hwWr->sz = 0;
			hwWr->ready = false;
		}
	}
	else
	{
		fifo[cAddr++] = symb;
		cAddr %= size;
		hwWr->sz++;
	}
}

void TextRxCircle::GetMessage(char* uBuff)
{
	if(usrRd->ready)
	{	
		usrRd->ready = false;
		unsigned int a = usrRd->bgAddr, n = usrRd->sz ;
		usrRd->sz = 0;				
		for(int i = 0; i < n; ++i)
		{
			uBuff[i] = fifo[a++];
			a %= size;				
		}
		usrRd = usrRd->next;
	}
}


TextRxCircle::~TextRxCircle()
{
	delete [] fifo;
	for (int i = 0; i < (nB - 1); ++i)
	{
		usrRd = hwWr->next;
		delete hwWr;
		hwWr = usrRd;
	}
	delete hwWr;
}

//////////////////////////////==================TextMessageTx========================///////////////////
TextMessageTx::TextMessageTx()
{
	buf = 0;
	isReady = false;
	ptr = 0;
	next = 0;
	bufSz = 0;
}


char TextMessageTx::SendSym()
{
	char ret = 0;
	if(isReady == false)
	{
		if ( ptr < bufSz )		
			ret =  buf[ptr];
		if(++ptr >= bufSz)
			isReady = true; 
	}
	return ret;
}

void TextMessageTx::CopyMessage(const char* wr)
{	
	bufSz = 0;
	while (wr[bufSz++] != END_TX)
	{
	}	
	bufSz--;
	buf = new char[bufSz];	
	for(int i = 0; i < bufSz; ++i)	
		buf[i] = wr[i];		
}

TextMessageTx* TextMessageTx::GetNext()
{
	return next;
}

void TextMessageTx::SetNext(TextMessageTx* nM)
{
	next = nM;
}

bool TextMessageTx::IsReady()
{
	return isReady;
}

TextMessageTx::~TextMessageTx()
{
	if(buf != 0)
		delete [] buf;
}

///((in[i] >= '0') && (in[i] <= '9'))  || 


bool Board::GetCmd(const char* in, char* cmd, int* prm, int len)
{
	bool isCmd = false;
	unsigned int pIn, pOut, bgCmd; 
	char prmStr[16];
		
	for(int i = 0; i < (len - 1) ; ++i)
	{
		if(((in[i] >= 'A') && (in[i] <= 'Z')) || ((in[i] >= 'a') && (in[i] <= 'z')))
		{
			bgCmd = i;
			isCmd = true;
			i = len;
		}		
	}
	if(isCmd)
	{
		pIn = bgCmd;
		pOut = 0;
		while( (((in[pIn] >= '0') && (in[pIn] <= '9'))  || ((in[pIn] >= 'A') && (in[pIn] <= 'Z')) || ((in[pIn] >= 'a') && (in[pIn] <= 'z')))
					&& (pIn < (len - 4)) )
		{
			cmd[pOut++] = in[pIn++];
		}	
		cmd[pOut] = 0;
		
		while((in[pIn] < '1') || (in[pIn] > '9'))
			pIn++;
		
		pOut = 0;
		while((in[pIn] >= '0') && (in[pIn] <= '9'))
			prmStr[pOut++] = in[pIn++];
			
		prmStr[pOut] = 0;	
		
		
		*prm = atoi(prmStr);
	}
	return isCmd;
}





// 	if(isCmd)
// 	{
// 		isCmd = false;
// 		for (int i = pIn; i < (len - 3); i++)
// 		{
// 			if(in[i] == '(')
// 			{
// 				prmBg = i;
// 				i = len;
// 				isCmd = true;
// 			}
// 		}	
// 	}

// 	if(isCmd)
// 	{
// 		isCmd = false;
// 		for (int i = prmBg; i < (len - 2); i++)
// 		{
// 			if(in[i] == ')')
// 			{
// 				prmEnd = i;
// 				i = len;
// 				isCmd = true;
// 			}
// 		}	
// 	}

// 	if(isCmd)
// 	{
// 		isCmd = false;
// 		for (int i = prmEnd; i < (len - 1); i++)
// 		{
// 			if(in[i] == ';')
// 			{
// 				isCmd = true;
// 			}
// 		}	
// 	}

// 	if(isCmd)
// 	{		
// 		isCmd = false;
// 		nPrm = 1;
// 		for (int i = (prmBg + 2); i < (prmEnd -1); i++)
// 		{
// 			if(in[i] == ',')
// 			{
// 				++nPrm;
// 			}
// 		}	
// 	}
// 	
// 	for(int i = 0; i = nPrm; ++i)
// 	{
// 		char
// 	
// 	}




// 	

// 	for(int i = 0; i < (len - 2) ; ++i)
// 	{
// 		if(in[i] == BG_CMD)
// 		{
// 			bgCmd = i + 1;			
// 			isCmd = true;
// 			i = len;
// 		}
// 		
// 	}

