#ifndef BOARD_CONFIG
#define BOARD_CONFIG

#include <AbstractTmetr.h>
//#include <AbstractUART.h>
#include <AbstractLED.h>
#include <AbstractButton.h>
#include <AbstractDisplay.h>
#include <AbstractEncoder.h>
#include <AbstractSerialIO.h>

namespace Board
{	
	AbstractTmetr* GetTmetr();
	
	AbstractIoSSP* GetJoystick();

	AbstractIoSSP* GetLDDac();

	AbstractIoSSP* GetTCDac();

	AbstractIoSSP* GetContrastDac();
	
	
	AbstractLED*  GetRelayControl ();
	
	AbstractLED*  GetLedControl ();
	
		
	AbstractButton* GetOnOffButton ();
	
	
	AbstractEncoder*  GetFrontPanelEncoder();


	AbstractButton* GetFrontPanelEncoderButton ();
		
	AbstractButton* GetUpButton (); 

	AbstractButton* GetDownButton (); 

	AbstractButton* GetLeftButton (); 

	AbstractButton* GetRightButton (); 

	AbstractButton* GetOkButton (); 


	AbstractLED* GetUpLED ();

	AbstractLED* GetDownLED ();

	AbstractLED* GetLeftLED ();

	AbstractLED* GetRightLED ();

	AbstractLED* GetOkLED ();


 

	

	AbstractDisplay* GetDisplay();
	
	
//	AbstractUART* GetUART0();
}

#endif

