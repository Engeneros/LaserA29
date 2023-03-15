#ifndef SERIAL_IO_H
#define SERIAL_IO_H
#include <AbstractSerialIO.h>
#include <uc.h>
using namespace uController;
namespace Board
{
	class Spi595_165 : public AbstractSerialIO
	{
	friend void Spi595_165_Router(Spi595_165* spiIO);	
	public:
		Spi595_165(unsigned char intfc, Gpio* ledOutClk, Gpio* buttL_Sr, unsigned long isrAddr);
		virtual unsigned int GetState() const;
		virtual void SetState(unsigned int state, unsigned int bMask = 0xffffffff);
		~Spi595_165();
	private:
		unsigned char sIfc;
		unsigned int bts;
		unsigned int leds;
		Gpio* ledsOClk;
		Gpio* bIOCtl;
		volatile unsigned short* dataReg;
		volatile unsigned char* statusReg;
	};	
	void Spi595_165_Router(Spi595_165* spiIO);


	class IoSSP : public AbstractIoSSP 
	{
	public: 
		IoSSP(AbstractSSP* si, unsigned char portGroup, unsigned char portNum, unsigned long isrA = 0);
		virtual void SspAdj ();
		virtual void Set(unsigned short val);
		virtual unsigned short Get();
		virtual void BuffWr(unsigned short val);
		virtual void SetState(unsigned short state, unsigned short bMask = 0xffff);
		virtual unsigned short BuffRd();
		virtual void Out();
		virtual void In();
		virtual void Select();
		virtual void Release();
		virtual ~IoSSP();
		virtual AbstractIoSSP* Next();
		virtual void SetNext(AbstractIoSSP* nx);
		virtual AbstractIoSSP* Prev();
		virtual void SetPrev(AbstractIoSSP* pr);
	protected:
		virtual void GpioCfg (unsigned char pG, unsigned char pN);
		virtual void SspCfg (unsigned long iA = 0);
		AbstractSSP* ssp;
		volatile unsigned long* ioSetRg;
		volatile unsigned long* ioClrRg;
		unsigned long selMsk;
		unsigned short tg, rdt;
		AbstractIoSSP* next;
		AbstractIoSSP* prev;
	};


	class J2 : public IoSSP
	{
	public: 
		J2( unsigned char pGLout, unsigned char pNLout,
						AbstractSSP* si, unsigned char portGroup, unsigned char portNum, unsigned long isrA = 0);
		virtual void SspAdj ();
		virtual void Select();
		virtual void Release();
		virtual ~J2();
	protected:
		virtual void AddPinCfg (unsigned char pGL,  unsigned char pNL);
		volatile unsigned long* ioSetRgLedOut;
		volatile unsigned long* ioClrRgLedOut;
		unsigned long ledOutMsk;		
	};

	////////////////////////////////
	class AD7888 : public IoSSP 
	{
	public: 
		AD7888(AbstractSSP* si, unsigned char portGroup, unsigned char portNum, unsigned char ch, unsigned long isrA = 0);
	//	virtual void SspCfg (unsigned long iA = 0);
		virtual void SspAdj ();
		virtual unsigned short Get();
		virtual ~AD7888();
	};

	class AD5200 : public IoSSP 
	{
	public: 
		AD5200(AbstractSSP* si, unsigned char portGroup, unsigned char portNum,  unsigned long isrA = 0);
		virtual void SspAdj ();
		virtual ~AD5200();
	};

	class AD5235 : public IoSSP 
	{
	public: 
		AD5235(AbstractSSP* si, unsigned char portGroup, unsigned char portNum, unsigned char chan, unsigned long isrA = 0);
		virtual void BuffWr(unsigned short val);
		virtual void Set(unsigned short val);
		virtual void SspAdj();	
		virtual void Release();
		virtual	void Out();
		virtual AbstractIoSSP* Next();
		virtual void SetNext(AbstractIoSSP* nx);
		virtual AbstractIoSSP* Prev();
		virtual void SetPrev(AbstractIoSSP* pr);
		virtual ~AD5235();
	protected:
		unsigned long rlsMsk[2];
		unsigned char sz[2];
		unsigned char  byteCnt;
		unsigned short data[2];
		AbstractIoSSP *nxtD[2];
		AbstractIoSSP *prvD[2];
	};

	class AD5662 : public AD5235
	{
	public: 
		AD5662(AbstractSSP* si, unsigned char portGroup, unsigned char portNum,  unsigned long isrA = 0);
		virtual void SspAdj ();

		virtual ~AD5662();
	};

}



#endif
