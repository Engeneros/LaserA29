#ifndef BUTTONS_H
#define BUTTONS_H
#include <AbstractButton.h>
#include <AbstractSerialIO.h>
#include <uc.h>
using namespace uController;
namespace Board
{
	class SerialButton : public AbstractButton
	{
	public:
		SerialButton(AbstractSerialIO* dev, unsigned int butNum);//unsigned int butNum);//
		virtual bool IsPressed() const;
		~SerialButton();
	private:
		unsigned int mask;
		AbstractSerialIO* d;
	};

	class SSPButton : public AbstractButton
	{
	public:
		SSPButton(AbstractIoSSP* dev, unsigned int butNum);//unsigned int butNum);//
		virtual bool IsPressed() const;
	private:
		unsigned short mask;
		AbstractIoSSP* d;
	};

	
	class PinButton : public AbstractButton
	{

	public:
		PinButton(Gpio* p);//unsigned int butNum);//
		virtual bool IsPressed() const;
		~PinButton();
	private:
		Gpio* pin;
	};

	class PinButtonRevers : public AbstractButton
	{

	public:
		PinButtonRevers(Gpio* p);//unsigned int butNum);//
		virtual bool IsPressed() const;
		~PinButtonRevers();
	private:
		Gpio* pin;
	};
}

#endif
