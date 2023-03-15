#include <BoardDevices.h>
#include <TMetr.h>
#include <lpc2148_ex.h>
#include <uC.h>
#include <SerialIO.h>
//#include <UARTs.h>
#include <leds.h>
#include <buttons.h>
#include <Displays.h>
#include <Encoders.h>
#include <SerialIO.h>
#include <TMetr.h>

using  namespace Board;
using namespace uController;

////TMetr
void __irq  TmetrEvent(void);
AbstractTmetr* tSens = new Tmp05(0, 21, 1, 3,  (unsigned long) TmetrEvent);

void __irq  TmetrEvent(void)
{
	tSens->IntEvent();
}

AbstractTmetr*	Board::GetTmetr()
{
	return tSens;
}

//////////////////// SSP1
void __irq  iS1(void);
class SSP1_Configurer
{
public:
	SSP1_Configurer();
};

AbstractSSP* s1 = new Ssp(SSP1_BASE_ADDR); 
AbstractIoSSP* jo = new J2(0, 11, s1, 0, 16, (unsigned long) iS1);
AbstractIoSSP* ldDA = new AD5662(s1, 0, 2, (unsigned long) iS1);
AbstractIoSSP* tcDA = new AD5662(s1, 0, 10, (unsigned long) iS1);
AbstractIoSSP* contrDA = new AD5662(s1, 0, 23, (unsigned long) iS1);
AbstractIoSSP* cDev = ldDA;
SSP1_Configurer ssp1Start;

void __irq  iS1(void)
{
 	cDev->Release ();
	cDev->Prev ()->In ();
	cDev = cDev->Next ();
 	cDev->SspAdj ();
	cDev->Out ();   	
	VICVectAddr = 0;    
}

SSP1_Configurer::SSP1_Configurer()
{
	PCfg (0, 17, 2);
	PCfg (0, 18, 2);
	PCfg (0, 19, 2);
	
	tcDA->BuffWr (0);
	ldDA->BuffWr (0);
	contrDA->BuffWr (0);
	jo->BuffWr (0);

	tcDA->SetNext (ldDA);
	ldDA->SetNext (contrDA);
	contrDA->SetNext (jo);
	jo->SetNext (tcDA);
	
	tcDA->SetPrev(tcDA);
	ldDA->SetPrev(ldDA);
	contrDA->SetPrev(contrDA);
	jo->SetPrev(jo);
	
	cDev->SspAdj ();
	cDev->Out ();
}

AbstractIoSSP* Board::GetJoystick()
{
	return jo;
}

AbstractIoSSP* Board::GetLDDac()
{
	return ldDA;
}

AbstractIoSSP* Board::GetTCDac()
{
	return tcDA;
}

AbstractIoSSP* Board::GetContrastDac()
{
	return contrDA;
}


///////////////================On Off logic out================//////////////////
Gpio* relCtlPin = new Gpio(1, 30,  true);
PinLEDRevers* relCtl = new PinLEDRevers(relCtlPin);

AbstractLED*  Board::GetRelayControl ()
{
	return relCtl;
}

Gpio* ledCtlPin = new Gpio(1, 31,  true);
PinLED* ledCtl = new PinLED(ledCtlPin);

AbstractLED*  Board::GetLedControl ()
{
	return ledCtl;
}
// // ///////////////================Logic inpus================//////////////////
Gpio* onButtPin = new Gpio(1, 29,  false);
PinButton* onButt = new PinButton(onButtPin);

AbstractButton*  Board::GetOnOffButton ()
{
	return onButt;
}
/////////////////////////Encoder
void __irq Enc_Router (void);
Cap* c0 = new Cap(0, 2, 0, 28, true, false, 8, 10000);// (unsigned long) CapI, true);
Cap* c1 = new Cap(0, 3, 0, 29, true, false, 8, 10000);//, (unsigned long) CapI, true);
Gpio* gpB = new Gpio(0, 7, false);
Gpio* gpA = new Gpio(0, 22, false);	

Encoder* en = new Encoder (c1, c0, gpA, gpB, (unsigned long) Enc_Router);

AbstractEncoder*  Board::GetFrontPanelEncoder() 
{
	return en;
}

void __irq Enc_Router (void)
{	
	EncGlobRouter(en);
}

/////////////////



// ///////////////////============FrontPanelJoystick===========////////////////

// // // void __irq Joy_IsrSSP(void);
// // // Gpio* ledClk = new Gpio(4, 16, true);
// // // Gpio* buttCtl = new Gpio(4, 19, true);
// // // Spi595_165* joystick = new Spi595_165(1, ledClk, buttCtl, (unsigned long)  Joy_IsrSSP);

// // void __irq Joy_IsrSSP(void)
// // {
// // 	Spi595_165_Router(joystick);
// // }

// // AbstractSerialIO*  GetFrontPanelJoystick()
// // {
// // 	return joystick;
// // }

// ///////////////================FrontPanelJoystick Buttons================//////////////////
AbstractButton* bUp = new SSPButton(jo, 2);
AbstractButton* bDown = new SSPButton(jo, 3);
AbstractButton* bLeft = new SSPButton(jo, 0);
AbstractButton* bRight = new SSPButton(jo, 1);
AbstractButton* bOk = new SSPButton(jo, 4);


AbstractButton*  Board::GetUpButton ()
{
	return bUp;
}

AbstractButton*  Board::GetDownButton ()
{
	return bDown;
}

AbstractButton*  Board::GetLeftButton ()
{
	return bLeft;
}

AbstractButton*  Board::GetRightButton ()
{
	return bRight;
}

AbstractButton*  Board::GetOkButton ()
{
	return bOk;
}


// ///////////////================FrontPanelJoystick LEDs================//////////////////
AbstractLED* lUp = new SSPLED (jo, 2);
AbstractLED* lDown = new SSPLED (jo, 3);
AbstractLED*  lLeft = new SSPLED (jo, 0);
AbstractLED*  lRight = new SSPLED (jo, 1);
AbstractLED*  lOk = new SSPLED (jo, 4);

AbstractLED*  Board::GetUpLED ()
{
	return lUp;	
}

AbstractLED*  Board::GetDownLED ()
{
	return lDown;
}

AbstractLED*  Board::GetLeftLED ()
{
	return lLeft;
}

AbstractLED*  Board::GetRightLED ()
{
	return lRight;
}

AbstractLED*  Board::GetOkLED ()
{
	return lOk;
}


////////////======================FrontPanelEncoder  Button===========////////////////

Gpio* encPin = new Gpio(0, 30,  false);
PinButton* encButt = new PinButton (encPin);

AbstractButton*  Board::GetFrontPanelEncoderButton ()
{
	return encButt;
}




///////////////////


//Display
	 
AbstractDisplay* dr = new EA122();
AbstractDisplay* Board::GetDisplay()
{
	return dr;
}
////////////////////////////////////////////////////////////////////////////////



/////////////////================UARTS================//////////////////

//void __irq Uart0ISR(void);

//Uart* u0 = new Uart (0, 0, 0, (unsigned long) Uart0ISR);

//void __irq  Uart0ISR(void)
//{
//	u0->IntEvent();
//}


//AbstractUART*  Board::GetUART0()
//{
//	return u0;
//}

//////////////////////////////===========================//////////////////////////

