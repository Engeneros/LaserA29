#include <leds.h>

using namespace Board;

SerialLED::SerialLED(AbstractSerialIO* dev, unsigned int ledNum)//unsigned int butNum)   //(
{
	d = dev;
	mask = 1 << ledNum;
	SetOn(false);
}

bool SerialLED::IsOn() const
{
	return ledSt;//((ledSt & mask) == 0)? false : true;//false;//
}

SerialLED::~SerialLED()
{

}

void SerialLED::SetOn(bool on)
{
	ledSt = on;
	unsigned int tmp;
	tmp = (on)? mask : 0; 
	d->SetState(tmp, mask);
}

void SerialLED::Toggle()
{
	ledSt = !ledSt;
	SetOn(ledSt);
}

SSPLED::SSPLED(AbstractIoSSP* dev, unsigned int ledNum)//unsigned int butNum);//
{
	d = dev;
	mask = 1 << ledNum;
	SetOn(false);
}
bool SSPLED::IsOn() const
{
	return ledSt;
}
void SSPLED::SetOn(bool on)
{
	ledSt = on;
	unsigned int tmp;
	tmp = (on)? mask : 0; 
	d->SetState(tmp, mask);
}
void SSPLED::Toggle()
{
	ledSt = !ledSt;
	SetOn(ledSt);	
}

PinLED::PinLED(Gpio* p)
{
	pin = p;
	ledSt = false;
	SetOn(ledSt);
}

bool PinLED::IsOn() const
{
	return ledSt;
}


void PinLED::SetOn(bool on)
{
	ledSt = on;
	pin->SetState(on);
}

void PinLED::Toggle()
{
	ledSt = !ledSt;
	SetOn(ledSt);
}

PinLED::~PinLED()
{

}

void PinLEDRevers::SetOn(bool on)
{
	ledSt = on;
	pin->SetState(!on);
}

void PinLEDRevers::Toggle()
{
	ledSt = !ledSt;
	SetOn(ledSt);
}



PinLEDRevers::PinLEDRevers(Gpio* p)
{
	pin = p;
	ledSt = false;
	SetOn(ledSt);
}

bool PinLEDRevers::IsOn() const
{
	return ledSt;
}


PinLEDRevers::~PinLEDRevers()
{

}


