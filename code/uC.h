#ifndef U_C_H
#define U_C_H

namespace uController
{

	class Led
	{
	public:
		Led(unsigned char portGroup, unsigned char portNum);
		void Toggle(void);
		void High(void);
		void Low(void);
		void Off(void);
		virtual ~Led(void);
	protected:
		unsigned char group, num;	
	};

	////////////////               == Port
	extern void PCfg(unsigned char portNumM, unsigned char portNumL, unsigned char func);

	extern void PDir(unsigned char portNumM, unsigned char portNumL, bool isOut);

	extern void PHigh(unsigned char portNumM, unsigned char portNumL);

	extern void PLow(unsigned char portNumM, unsigned char portNumL);

	extern void PTg(unsigned char portNumM, unsigned char portNumL);
	bool  PGet(unsigned char portNumM, unsigned char portNumL)	;

	//////////////               Port ==


	//////////////////////////// == I2C 
	extern  void  I2cIni(unsigned char ifcNum);

	extern unsigned char I2cStart(unsigned char ifcNum);

	extern unsigned char  I2cStopWr (unsigned char ifcNum);

	extern unsigned char  I2cStopRd ( unsigned char ifcNum);

	extern unsigned char  I2cRawRead (unsigned char ifcNum, unsigned char &byte);

	extern unsigned char  I2cRawWrite (unsigned char ifcNum, unsigned char byte);

	extern int  I2cGetByte(unsigned char ifcNum, unsigned char dev, unsigned char addr, unsigned char &data);

	extern int  I2cSendByte(unsigned char ifcNum, unsigned char dev, unsigned char addr, unsigned char data);
	//////////////////////////// I2C ==

	/////   == SSP 
	extern void  SSPPinCfg(unsigned char ifcNum);

	extern void   SSPSetDataSize(unsigned char ifcNum, unsigned char n);

	extern void   SSPSetfronts(unsigned char ifcNum, unsigned char f);

	extern void  SSPEnable(unsigned char ifcNum);

	extern void   SSPIni(unsigned char ifcNum, unsigned char dSz, unsigned char clkDiv, unsigned char fronts, unsigned long isrAddr = 0);

	extern unsigned short  SSPExchange(unsigned char ifcNum, unsigned short data);

	extern unsigned short  SSPFastExchange(unsigned char ifcNum, unsigned short data);

	extern void  SSPWrite(unsigned char ifcNum, unsigned short data);

	void  SSPFastWrite(unsigned char ifcNum, unsigned short data);
	void  SSPWriteOnly(unsigned char ifcNum, unsigned short data);

	extern unsigned short  SSPRead(unsigned char ifcNum);


	extern unsigned short  SSPStartAndRead(unsigned char ifcNum);
	extern void inline SSPWait(unsigned char ifcNum)  ;
	extern void SSPSetDivs(unsigned char ifc, unsigned long div);
	extern void SSPSetCtlRegDiv(unsigned char ifc, unsigned char div);
	extern void SSPSetPCLKDiv(unsigned char ifc, unsigned char div);


	extern bool SSPTxFifoIsEmpty(unsigned char ifc);


	/////    SSP ==


	//////////   == I2S
	extern  void I2sIni(void);
	extern  void I2sIni(unsigned long isrAddr, unsigned char level);
	extern  void I2sIntDisable();
	extern void I2sIntEnable();

	extern  unsigned int I2sRead (void);
	//////////   I2S ==






	/////////////// fixxx
	void PwmSEIni(unsigned int T, unsigned int duty);

	void PwmSESetDuty(unsigned int  duty);

	////////////

	///////////// ==========RTC
	void RtcStart();

	void RtcStop();

	void  RtcIni(void);

	void RtcSetSec(unsigned char val);

	unsigned char RtcGetSec();

	void RtcSetMin(unsigned char val);

	unsigned char RtcGetMin();

	void RtcSetHour(unsigned char val);

	unsigned char RtcGetHour();

	void RtcSetDay(unsigned char val);


	unsigned char RtcGetDay();

	void RtcSetMonth(unsigned char val);

	unsigned char RtcGetMonth();


	void RtcSetYear(unsigned short val);


	unsigned short RtcGetYear();


	///////////// RTC==================


	///////////////////////////==================uart=====================////////////////////////
	void uartIni(unsigned char uartNum = 0, bool rxEn = false, bool txEn = false, unsigned long isrAddr = 0);

	void uartSend(unsigned char n, unsigned char b);

	char UartGet(unsigned char n);
			
	void UartSetIsrAdd(unsigned char n, unsigned long isrAddr);
	void UartDisableInt(unsigned char n);

	unsigned short UartGetIntSrc(unsigned char n);
	 
	bool UartTxIsReady(unsigned char n) ;

	////////////////////////////////Timer=====================
	void  IntervalGenerator(unsigned char nT, unsigned int nClk, unsigned long isrAddr);
	void TimerStop(unsigned char nT);

	void TimerStart(unsigned char nT);

	void TimerReset(unsigned char nT);

	////////////////////////////////====================Timer


	///////////////////=============ADC=============================
	void LPCadcIni(unsigned char num = 0);

	void LPCadcChOn(unsigned char ch, unsigned char num = 0);

	void LPCadcChOff(unsigned char ch, unsigned char num = 0);

	unsigned short LPCadcRead(unsigned char ch, unsigned char num = 0);

	////////   == DAC
	void DacIni(void);
	void DacOut(unsigned short data);
	////////////////   DAC ==


	//extern inline void lpcDacIni(void);



	/////////////////////////////////==========2013===========/////////////////////


	unsigned long GetPclkDiv(unsigned int div);
	//////========= Capture===
	class Cap
	{
	public:
		static void __irq CapI1 (void);
		Cap(unsigned char nTim, unsigned char nCap, unsigned char pG, unsigned char pN,
		bool isFCap, bool isRCap, unsigned char pclkDiv, unsigned int presc, unsigned long isr = 0,  bool intEn = false);
		void DisableCap(void);
		void EnableFCap(void);
		void EnableRCap(void);
		void EnableFRCap(void);
		void EnableInt(void);
		void DisableInt(void);
		void SetIsrAddr(unsigned long addr);
		void SetPclkDiv(unsigned int div);
		void SetPrescaler(unsigned int pr);
		unsigned int GetT(void);
		bool IsCap(void);
		void ResetFlags(void);
		void EnableOnlyFCap(void);//capture only on falling edge
		void EnableOnlyRCap(void) ;//capture only on rising edge
		~Cap();
	private:	
		volatile unsigned long* tcCtlReg; 
		volatile unsigned long* capCtlReg;
		volatile unsigned long* ctlReg;
		volatile unsigned long* evFlagReg;
		volatile unsigned long* prescReg;
		volatile unsigned long* capReg;
		volatile unsigned long* vicVectAddrReg;
		volatile unsigned long* vicVectCntReg;
		unsigned char capNum;
		unsigned char intNum;
		unsigned char timNum;
	};

	/////////////==============Capture_end
	/////////////=============== GPIO=============
	class Gpio
	{
	public:
		Gpio(unsigned char portGroup, unsigned char portNum, bool isOut, unsigned long isr = 0);
		void Toggle(void);
		void High(void);
		void Low(void);
		void SetDir(bool dir);
		bool GetState(void);
		void SetState(bool state);
		void SetActivR(void);
		void SetActivF(void);
		void SetActivFR(void);
		void SetPassivR(void);
		void SetPassivF(void);
		void SetPassivFR(void);
		void SetIntAddr(unsigned long addr);
		virtual ~Gpio(void);
	protected:
		unsigned char group, num;	
	};
/////////////=============== GPIO End=============
/////////////=================SSP==================	
	class AbstractSSP
	{
	public:
		virtual void SetDataSize(unsigned char sz) = 0;
		virtual void Setfronts(unsigned char f) = 0;
		virtual void SetDiv(unsigned int div) = 0;
		virtual void EnableInterrups(unsigned long isrAddr, unsigned char intType) = 0;
		virtual void DisableInterrups() = 0;
		virtual void Enable() = 0;
		virtual void Disable() = 0;
		virtual void FastWr(unsigned short data) = 0;
		virtual unsigned short FastRd() = 0;
		virtual unsigned short FastRW(unsigned short data) = 0;
		virtual void Wait() = 0;
		virtual void Write(unsigned short data) = 0;
		virtual unsigned short Read() = 0;
		virtual unsigned short RW(unsigned short data) = 0;
		virtual unsigned long GetStatus() = 0; 
		virtual bool TxIsEmpty() = 0;
		virtual void Start() = 0;
	
	
	};

	class Spi : public AbstractSSP
	{
	public:
		Spi(unsigned long ifcStructAddr);
		~Spi();
		virtual void SetDataSize(unsigned char sz);
		virtual void Setfronts(unsigned char f);
		virtual void SetDiv(unsigned int div);
		virtual void EnableInterrups(unsigned long isrAddr, unsigned char intType);
		virtual void DisableInterrups();
		virtual void Enable();
		virtual void Disable();
		virtual void FastWr(unsigned short data);
		virtual unsigned short FastRd();
		virtual unsigned short FastRW(unsigned short data);
		virtual void Wait();
		virtual void Write(unsigned short data);
		virtual unsigned short Read();
		virtual unsigned short RW(unsigned short data);
		virtual unsigned long GetStatus();
		virtual bool TxIsEmpty();
		virtual void Start();
	private:
		unsigned long pIfc;
	};

	class Ssp : public AbstractSSP
	{
	public:
		Ssp(unsigned long ifcStructAddr);
		~Ssp();
		virtual void SetDataSize(unsigned char sz);
		virtual void Setfronts(unsigned char f);
		virtual void SetDiv(unsigned int div);
		virtual void EnableInterrups(unsigned long isrAddr, unsigned char intType);
		virtual void DisableInterrups();
		virtual void Enable();
		virtual void Disable();
		virtual void FastWr(unsigned short data);
		virtual unsigned short FastRd();
		virtual unsigned short FastRW(unsigned short data);
		virtual void Wait();
		virtual void Write(unsigned short data);
		virtual unsigned short Read();
		virtual unsigned short RW(unsigned short data);
		virtual unsigned long GetStatus();
		virtual bool TxIsEmpty();
		virtual void Start();
	private:
		unsigned long pIfc;
	};
/////////////=================SSP End==================	
}
#endif

