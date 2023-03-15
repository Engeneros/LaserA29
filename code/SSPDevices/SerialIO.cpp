#include <SerialIO.h>
#ifdef LPC2478
#include <LPC24xx_ex.h>
#endif
#ifdef LPC2148
#include <LPC2148_ex.h>
#endif

using namespace Board;

void Board::Spi595_165_Router(Spi595_165* spiIO)
{
	spiIO->bIOCtl->Low();
	spiIO->ledsOClk->Low();
	spiIO->ledsOClk->High();
	spiIO->bts = *(spiIO->dataReg); //SSPxDR;
	spiIO->bIOCtl->High();
	if(*(spiIO->statusReg) & 2) //tx FIFO is not full
		SSPFastWrite(spiIO->sIfc, spiIO->leds);
	VICVectAddr = 0;	
}

//////////========class Spi595_165
Spi595_165::Spi595_165(unsigned char intfc, Gpio* ledOutClk, Gpio* buttL_Sr, unsigned long isrAddr)
{	
	leds = 0;
	bts = 0xffffffff;
	sIfc = intfc;
#ifdef LPC2478	
	dataReg  = (sIfc == 0)? &SSP0DR : &SSP1DR;
	statusReg  = (sIfc == 0)? &SSP0SR : &SSP1SR;
#endif	
#ifdef LPC2148
	dataReg  =  &SSPDR;
	statusReg  =  &SSPSR;
#endif		
	ledsOClk = ledOutClk;
	bIOCtl = buttL_Sr;
	SSPIni(sIfc, 8, 254, 3, isrAddr);
	SSPSetCtlRegDiv(sIfc, 255);
	SSPSetPCLKDiv(sIfc, 8);	
	for(int i = 0; i < 5; i++)
		SSPFastWrite(sIfc, 0x55);
}

unsigned int Spi595_165::GetState() const
{	
	if(SSPTxFifoIsEmpty(sIfc))
		SSPFastWrite(sIfc, leds);
	return bts;
}

void Spi595_165::SetState(unsigned int state, unsigned int bMask)
{	
	unsigned int n = state & bMask;
	unsigned int o = leds & ~bMask;
	leds = n | o;
	if(SSPTxFifoIsEmpty(sIfc))
		SSPFastWrite(sIfc, leds);
}

Spi595_165::~Spi595_165()
{
	
}
//////////========class Spi595_165  end 	




IoSSP::IoSSP(AbstractSSP* si, unsigned char portGroup, unsigned char portNum, unsigned long isrA)			
{	
	GpioCfg(portGroup, portNum);
	ssp = si;
//	SspCfg (isrA);	
}


void IoSSP::GpioCfg (unsigned char pG, unsigned char pN)
{
	selMsk = 1 << pN;
	if(pG == 0)
	{
		ioSetRg = &IOSET0;
		ioClrRg = &IOCLR0;
	}
	else if(pG == 1)
	{
		ioSetRg = &IOSET1;
		ioClrRg = &IOCLR1;
	}
#ifdef LPC2478	
	else if(pG == 2)
	{
		ioSetRg = &FIO2SET;
		ioClrRg = &FIO2CLR;
	}
	else if(pG == 3)
	{
		ioSetRg = &FIO3SET;
		ioClrRg = &FIO3CLR;
	}
	else if(pG == 4)
	{
		ioSetRg = &FIO4SET;
		ioClrRg = &FIO4CLR;
	}
#endif	
	PCfg(pG, pN, 0);
	PDir(pG, pN, true);
	Release();
}

void IoSSP::SspCfg (unsigned long iA)
{
	ssp->Disable();
//	ssp->SetDataSize(8);
//	ssp->Setfronts(0);
	ssp->SetDiv(32 * 255);
// 	unsigned short tmp;
// 	for(int i  = 0; i < 8; i++)
// 		tmp = ssp->RW(0x5555);
	if(iA != 0)
		ssp->EnableInterrups(iA, 2);
	else
		ssp->DisableInterrups();
	ssp->Enable();
}

void IoSSP::SspAdj()
{
	ssp->SetDataSize(8);
	ssp->Setfronts(0);
}

void IoSSP::Select()
{
	*ioClrRg = selMsk;
}


void IoSSP::Release()
{
	*ioSetRg = selMsk;
}

void IoSSP::Set(unsigned short val)
{
	Select();
	SspAdj();
	ssp->Write(val);
	Release();
}

unsigned short IoSSP::Get()
{
	Select();
	SspAdj();
	unsigned short ret = ssp->Read();
	Release();
	return ret;
}

void IoSSP::BuffWr(unsigned short val)
{
	tg = val;
}

void IoSSP::SetState(unsigned short state, unsigned short bMask)
{	
	unsigned short n = state & bMask;
	unsigned short o = tg & ~bMask;
	tg = n | o;
}


unsigned short IoSSP::BuffRd()
{
	return rdt;
}

void IoSSP::Out()
{
	Select();
	ssp->FastWr(tg);
}

void IoSSP::In()
{
	rdt = ssp->FastRd();
}

AbstractIoSSP* IoSSP::Next()
{
	return next;
}

void IoSSP::SetNext(AbstractIoSSP* nx)
{
	next = nx;
}

AbstractIoSSP* IoSSP::Prev()
{
	return prev;
}

void IoSSP::SetPrev(AbstractIoSSP* pr)
{
	prev = pr;
}


IoSSP::~IoSSP()
{
}

J2::J2( unsigned char pGLout, unsigned char pNLout,
 					AbstractSSP* si, unsigned char portGroup, unsigned char portNum, unsigned long isrA)
					: IoSSP(si, portGroup, portNum, isrA)
{	
	SspCfg (isrA);	
	AddPinCfg ( pGLout,  pNLout);
}

void J2::AddPinCfg (unsigned char pGL,  unsigned char pNL)
{
	ledOutMsk = 1 << pNL;
	if(pGL == 0)
	{
		ioSetRgLedOut = &IOSET0;
		ioClrRgLedOut = &IOCLR0;
	}
	else if(pGL == 1)
	{
		ioSetRgLedOut = &IOSET1;
		ioClrRgLedOut = &IOCLR1;
	}
#ifdef LPC2478	
	else if(pGL == 2)
	{
		ioSetRgLedOut = &FIO2SET;
		ioClrRgLedOut = &FIO2CLR;
	}
	else if(pGL == 3)
	{
		ioSetRgLedOut = &FIO3SET;
		ioClrRgLedOut = &FIO3CLR;		
	}
	else if(pGL == 4)
	{
		ioSetRgLedOut = &FIO4SET;
		ioClrRgLedOut = &FIO4CLR;
	}
#endif	
	PCfg(pGL, pNL, 0);
	PDir(pGL, pNL, true);
	PHigh(pGL, pNL);
	Release();
}

void J2::SspAdj()
{
	ssp->SetDataSize(8);
	ssp->Setfronts(0);
}

void J2::Select()
{
	*ioSetRg = selMsk;
}

void J2::Release()
{
	*ioClrRgLedOut = ledOutMsk;
	*ioClrRg = selMsk;
	*ioSetRgLedOut = ledOutMsk;
}

J2::~J2()
{
}
////////////////////////////////

AD7888::AD7888(AbstractSSP* si, unsigned char portGroup, unsigned char portNum, unsigned char ch, unsigned long isrA)	: IoSSP(si, portGroup, portNum)
{	
	SspCfg (isrA);
	tg = (ch << 11) | (1 << 10);
}


void AD7888::SspAdj()
{
	ssp->SetDataSize(16);
	ssp->Setfronts(3);
}

unsigned short AD7888::Get()
{
	Select();
	SspAdj();
	unsigned short ret = ssp->RW(tg);
	Release();
	return ret;
}


 AD7888::~AD7888()
{
}


AD5200::AD5200(AbstractSSP* si, unsigned char portGroup, unsigned char portNum, unsigned long isrA)	: IoSSP(si, portGroup, portNum)
{	
	SspCfg (isrA);
}

void AD5200::SspAdj()
{
	ssp->SetDataSize(8);
	ssp->Setfronts(0);
}

 AD5200::~AD5200()
{
}

AD5235::AD5235(AbstractSSP* si, unsigned char portGroup, unsigned char portNum, unsigned char chan, unsigned long isrA)	: IoSSP(si, portGroup, portNum)
{
	byteCnt = 0;
	rlsMsk[1] = selMsk;
	rlsMsk[0] = 0;
	data[0] = 0xb0 | (chan & 1);
	nxtD[0] = prvD [1] = this;
	sz[0] = 8;
	sz[1] = 16;
	SspCfg (isrA); 
}


void AD5235::SspAdj()
{
	ssp->SetDataSize(sz[byteCnt]);
	ssp->Setfronts(2);
}

void AD5235::BuffWr(unsigned short val)
{
	data[1] = val;
}

void AD5235::Release()
{
	*ioSetRg = rlsMsk[byteCnt];
	
}


void AD5235::Set(unsigned short val)
{
	Select();
	//SspAdj();
	ssp->SetDataSize(8);
	ssp->Setfronts(2);
	ssp->Write(data[0]);
	ssp->SetDataSize(16);
	ssp->Write(val);//(val & 0x0300) >> 8);
//	ssp->Write(val & 0xff);	
	byteCnt = 1;
	Release();
}


void AD5235::Out()
{
	Select();
	ssp->FastWr(data[byteCnt]);
}


AbstractIoSSP* AD5235::Next()
{
	AbstractIoSSP* ret = nxtD[byteCnt++];
	byteCnt %= 2;
	return ret;
}

void AD5235::SetNext(AbstractIoSSP* nx)
{
	nxtD[1] = nx;
}

AbstractIoSSP* AD5235::Prev()
{
	return prvD[byteCnt];
}

void AD5235::SetPrev(AbstractIoSSP* pr)
{
	prvD[0] = pr;
}


AD5235::~AD5235()
{
}


AD5662::AD5662(AbstractSSP* si, unsigned char portGroup, unsigned char portNum, unsigned long isrA)	: AD5235(si, portGroup, portNum, 0, isrA)
{	
	data[0] = 0;
}


AD5662::~AD5662()
{	
}


void AD5662::SspAdj()
{
	ssp->SetDataSize(sz[byteCnt]);
	ssp->Setfronts(1);	
}



