#include <buttons.h>

using namespace Board;

SerialButton::SerialButton(AbstractSerialIO* dev, unsigned int butNum)//unsigned int butNum)   //(
{
	d = dev;
	mask = 1 << butNum;
}

bool SerialButton::IsPressed() const
{
	return ((d->GetState() & mask) == 0)? true : false;//false;//
}

SerialButton::~SerialButton()
{

}

SSPButton::SSPButton(AbstractIoSSP* dev, unsigned int butNum)//unsigned int butNum);//
{
	d = dev;
	mask = 1 << butNum;
}

bool SSPButton::IsPressed() const
{
	return ((d->BuffRd() & mask) == 0)? true : false;//false;//
}

PinButton::PinButton(Gpio* p)
{
	pin = p;	
}

bool PinButton::IsPressed() const
{
	return  (pin->GetState() == false)? true : false;
}

PinButton::~PinButton()
{
}

PinButtonRevers::PinButtonRevers(Gpio* p)
{
	pin = p;	
}

bool PinButtonRevers::IsPressed() const
{
	return  pin->GetState();
}

PinButtonRevers::~PinButtonRevers()
{
	
}


