#ifndef LEDS_H
#define LEDS_H
#include <AbstractLED.h>
#include <AbstractSerialIO.h>
#include <uc.h>
using namespace uController;
namespace Board
{
	class SerialLED : public AbstractLED
	{
	public:
		SerialLED(AbstractSerialIO* dev, unsigned int ledNum);//unsigned int butNum);//
		virtual bool IsOn() const;
		virtual void SetOn(bool on);
		virtual void Toggle();
		~SerialLED();
	private:
		unsigned int mask;
		AbstractSerialIO* d;
		bool ledSt;	
	};
	
	
	class SSPLED : public AbstractLED
	{
	public:
		SSPLED(AbstractIoSSP* dev, unsigned int ledNum);//unsigned int butNum);//
		virtual bool IsOn() const;
		virtual void SetOn(bool on);
		virtual void Toggle();
	private:
		unsigned int mask;
		AbstractIoSSP* d;
		bool ledSt;	
	};

	class PinLED : public AbstractLED
	{
	public:
		PinLED(Gpio* p);
		virtual bool IsOn() const;
		virtual void SetOn(bool on);
		virtual void Toggle();
		~PinLED();
	private:
		Gpio* pin;
		bool ledSt;
	};




	class PinLEDRevers : public AbstractLED
	{
	public:
		PinLEDRevers(Gpio* p);
		virtual bool IsOn() const;
		virtual void SetOn(bool on);
		virtual void Toggle();
		~PinLEDRevers();
	private:
		Gpio* pin;
		bool ledSt;
	};
}
#endif
