 #include <Delay.h>
 #include <stdIo.h>
 #include <string.h>
 #include <BoardDevices.h>
 #include <iap.h>
 #include <UInterfaces.h>
 #include <math.h>
 //#include <LPC2148_ex.h>
 #include <Converters.h>
 //#include <uc.h>
  
 using namespace Board;
 using namespace HwSleep;
 //using namespace uController;

class iRC {
private:
	double Uc, TAU;
public:	
	inline void setTau(double t) {TAU = t;}
	inline double getTau(void) {return TAU;}
	inline void setUc(double u) {Uc = u /(1.0 - TAU);}
	iRC(double tau, double u = 0.0) { setTau(tau); setUc(u);};
	inline double getUc(double Uin) { Uc *= TAU; Uc += Uin; return Uc * (1.0 - TAU);} 
	virtual ~iRC() {};
};

unsigned short TControl(double t, double tg)
{
	static double pt = 25.0, ctl = 0.0;
	double dt, dtTg, dtErr, dtTg2, s, s2;
	static const double K = 200.0, TAU  = 0.99, KS = 4000.0 * (1- TAU) ;	
	static iRC d (TAU), d2 (TAU) ; 
	
	dt = t - pt;
	pt = t;
	dtTg = tg - t;
	dtTg2 = dtTg * dtTg;
	dtErr = dtTg - dt * 100.0;
	s  = d.getUc(dtTg);
	s2 = d2.getUc(dtTg2);
	if((dtTg2 < 0.000625)  && (s2 < (0.0025 /(1 - TAU))) )
		ctl -= (KS * s +  dtErr * K);
	else
		ctl -= dtErr * K;
	if (ctl > 65535.0)
		ctl = 65535.0;
	else if (ctl < 0.0)
		ctl = 0.0;	
	return (unsigned short) ctl;
}

unsigned short ItoQ(double mA)//#26
{
	//a29
	const double Q00 = 2570;
	const double Q0 = 3040;
	const double I0 = 0.4 / 4.0;
	const double Q1 = 64000.0;//63700.0;///62000.0;//63000.0;
	const double I1 = 53.05/4.0;//53.6/4.0;//53.2/4.0;//53.02/4.0;//48.8 / 4.0;
	const double Q_MAX = 63300.0;
	//a26
//	const double Q00 = 4720;
//	const double Q0 = 5180;
//	const double I0 = 0.4 / 4.0;
//	const double Q1 = 65400.0;//63700.0;///62000.0;//63000.0;
//	const double I1 = 53.1/4.0;//53.6/4.0;//53.2/4.0;//53.02/4.0;//48.8 / 4.0;
//	const double Q_MAX = 65000.0;
	unsigned short ret =  ( mA * (Q1 - Q0) / (I1 - I0)  + Q00);
	if(ret > Q_MAX)
		ret = Q_MAX;
	else if(mA < 0.05)
		ret = 0;
	return ret;
}


double SlowTg(double tg)
{
	static const double D = 0.004;
	static double slow = 0;
	if(slow < tg)
		slow = ( (tg - slow) < (2 * D) )? tg : slow + D;
	else if(slow > tg)
		slow = ( (slow - tg) < (2 * D) )? tg : slow - D;
	return slow;
}

struct FlashBuf
{
	double oldT;
};

//void WDTrefresh()
//{
//      WDFEED = 0xaa;
//      WDFEED = 0x55;
//}

//__irq void  WDInt()
//{
//	PTg(1, 31);
//	WDTrefresh();
//	VICVectAddr = 0;
////		T0IR = 0x0f;
//}

//void WDTini (unsigned int time_in_ms)
//{      
//        unsigned int tics = time_in_ms  * 250 * 60;
//        WDMOD |= 1;
//        WDTC = tics;     
// //       WDCLKSEL   = 1;
//        WDFEED = 0xaa;
//        WDFEED = 0x55;
//}
//void  __irq T0Match (void)
//{
//	PTg(1, 31);
//	T0IR = 0xff;
//	VICVectAddr = 0;
//}
void StdCalibration(Converter* c)
{
	c->AddPoint(0.0, 0.0);
	c->AddPoint(0.6, 0.1);
	c->AddPoint(2.0, 1.0);
	c->AddPoint(6.0, 3.0);
	c->AddPoint(13.0, 9.0);  
}
int main()
{
		AbstractLED* rl = GetRelayControl ();	
		AbstractButton*  bt = GetOnOffButton ();
		////strong Off
		rl->SetOn(false);
		int offBarrier = 0;
		while (offBarrier < 100000)
		{
		if ( bt->IsPressed() == true)
			++offBarrier;
		else
			offBarrier = 0;
		}
	////strong Off end	
	 //Relay & Button

		AbstractLED* okLed = GetOkLED () ;
		AbstractLED* upLed =  GetUpLED ();
		AbstractLED* downLed = GetDownLED ();
		AbstractLED* leftLed = GetLeftLED ();
		AbstractLED* rightLed = GetRightLED ();
		AbstractLED* led = GetLedControl ();
		
		AbstractLED* fpLeds[] = {upLed, rightLed, downLed, leftLed, okLed};
		bool off = true;

		SleepMs(100);	
    ////TMetr
        AbstractTmetr* t = GetTmetr();
        iRC trc (0.975);
        double tf = 0.0;
    ////	SSP1 Devices
        AbstractIoSSP* ldDac = GetContrastDac();
        AbstractIoSSP* tcDac = GetTCDac();
 //       AbstractIoSSP* contrDac = GetContrastDac();
 		
    ////////////Display
        double to = 0.0;
		double p = 0.0, prevP = 0.0;
        double iTg = 0.0, pTg  = 0.0, iTgPrev = 0.0, pTgPrev = 0.0, pClb = 0.0;
        double iTgErr = 0.0;
        double iSlowTg = 0.0, prevISlowTg = 0.0;
        double tTg = 25.0;
        bool tOk = true;
        const double I_MAX = 9.999;
        const double I_MIN = 0.0;
        const double T_MAX = 29.0;
        const double T_MIN = 21.0;
        const double T_ERR = 30.5;
		const double P_MAX = 6.0;
		const double P_MIN = 0.0;
//		double pMax = 9.0;
//		double pMin = 0;
			
		double  dI;
		const unsigned int SPEED_DIV_MAX = 400;
		const unsigned int SPEED_DIV_MIN = 4;
		const unsigned int SPEED_DIV_AHTUNG = 10;
		unsigned int curSpeedDiv = SPEED_DIV_AHTUNG * 2,  prevWSpeedDiv = curSpeedDiv;
		const double TIME_MAX = SPEED_DIV_MAX  * (I_MAX - I_MIN) / 10.0;
		const double TIME_MIN = 0.0;
		double  tmpTime = 0.0,  timeInput = 0.0;
		AbstractDisplay* displ = GetDisplay();
		
		tTg = (T_MAX + T_MIN) / 2.0;
////////======FlashSaver ===========
		const unsigned int SAVE_SZ = MAX_CLBR_POINTS * 2 + 3 ;
		Converter* cnv = new Converter (0.5, 13.0, 9.0);	
        theSaver sv = theSaver(60000); 
		double flashBuf[SAVE_SZ];
        sv.remember((unsigned long) &flashBuf, (char*) &flashBuf, sizeof(flashBuf));   
		unsigned int nPoints = (unsigned int) flashBuf[0];//*clbSz;
		if((nPoints > 1) && (nPoints < 20))
		{	
			double tmpT = flashBuf[1];
			if((tmpT <= T_MAX) && (tmpT >= T_MIN))
				tTg = tmpT;
			if(nPoints > 2)
			{
				tmpT = flashBuf[2];
				if(((int) tmpT <= SPEED_DIV_MAX) && ((int) tmpT >= SPEED_DIV_MIN))
					curSpeedDiv = prevWSpeedDiv = (int) tmpT;
			}
			if(nPoints > 3)
			{
				double tmpX, tmpY;
				for(int i = 3; i < nPoints; ++i)
				{
					tmpX = flashBuf[i];
					tmpY = flashBuf[++i];
					if((tmpX <= I_MAX) && (tmpX >= I_MIN) && (tmpY <= P_MAX) && (tmpY >= P_MIN))
						cnv->AddPoint(tmpX, tmpY);
				}		
			}
			else
				StdCalibration(cnv);
		}
		else
			StdCalibration(cnv);
////////////////////////////////=================INTERFACE INICIALIZATION==============//////////////////////////////////////////////////		
		bool tgIChStart = false, startFlPrev = false; 
		bool siFl = false, stecFl = false, stiFl = false, spFl = false, stepFl = false, stipFl = false;
 ///============CURRENT WORK PANEL=====#############  =========         
        OutPrm* opC = new OutPrm(0, 0, ARIALCYR28, 2, false, 3/*, "A "*/);//ARIALCYR28, 1, false, 2, "6 ");
        opC->SetInputLink(&iSlowTg);//&curr);
		OutPrm* unitAmp = new OutPrm(79, 10, MIDDLE_FONT, 1, false);
        unitAmp->SetInputLink(0);	
        unitAmp->SetVal("AMPERE ");
        OutPrm* opT = new OutPrm(70, 0, MIDDLE_FONT, 2, false, 2, "<C ");
        opT->SetInputLink(&to);	
		OutPrm* opP = new OutPrm(70, 20, MIDDLE_FONT, 2, false, 3, "W ");
        opP->SetInputLink(&p);	

        OutPrm* opWork = new OutPrm(70, 10, MIDDLE_FONT, 2, false);
        opWork->SetInputLink(0);	
        opWork->SetVal("  ");

        CPanel* w = new WorkPanel();
        w->AddPrm(opC);
		w->AddPrm(unitAmp);
		w->AddPrm(opP);//opTest2);
		w->AddPrm(opT);
		w->AddPrm(opWork);
		w->SetMax(I_MAX);
		w->SetMin(I_MIN);		
//=================POWER WORK PANEL==================	
		OutPrm* opPPow = new OutPrm(0, 0, ARIALCYR28, 2, false, 3/*, "W "*/);//ARIALCYR28, 1, false, 2, "6 ");
		opPPow->SetInputLink(&p);//&curr);	
		OutPrm* unitWatt = new OutPrm(79, 10, MIDDLE_FONT, 1, false);
        unitWatt->SetInputLink(0);	
        unitWatt->SetVal("WATT  ");		
		OutPrm* opCPow = new OutPrm(70, 20, MIDDLE_FONT, 2, false, 3, "A ");
		opCPow->SetInputLink(&iSlowTg);	

		CPanel* wP = new WorkPanel();
		wP->AddPrm(opPPow);
		wP->AddPrm(unitWatt);
		wP->AddPrm(opCPow);
		wP->AddPrm(opT);
		wP->AddPrm(opWork);
		wP->SetMax(P_MAX);
		wP->SetMin(P_MIN);
//=================CURRENT SET PANEL@@@@@@@
		OutPrm* tc = new OutPrm(0, 0, ARIALCYR28, 2, false, 3);  //, "A ");
		tc->SetInputLink(&iTg);
		OutPrm* tti = new OutPrm(70, 0, MIDDLE_FONT, 2, false, 2, "<C ");
		tti->SetInputLink(&tTg);
		OutPrm* tTimeC = new OutPrm(70, 10, MIDDLE_FONT, 2, false, 0, " S   ");
		tTimeC->SetInputLink(&tmpTime);
		OutPrm* setI = new OutPrm(70, 20, MIDDLE_FONT, 2, false);
		setI->SetInputLink(0);	
		setI->SetVal("CURRENT");        

		CPanel* si = new TargetPanel(0.001, 0.1, 1.0);
		si->AddPrm(tc);
		si->AddPrm(tti);		
		si->AddPrm(setI);
		si->AddPrm(tTimeC);
		si->SetMax(I_MAX);
		si->SetMin(I_MIN);
		si->SetOutputFlag(&siFl);
 //==================TEMPERATURE (C-MODE) SET PANEL !!!!!!!!!!!!!!!!!!!!       
		OutPrm* tt = new OutPrm(0, 0, ARIALCYR28, 2, false, 2);
		tt->SetInputLink(&tTg);
		OutPrm* tci = new OutPrm(70, 0, MIDDLE_FONT, 2, false, 3, "A ");
		tci->SetInputLink(&iTg);
		OutPrm* setT = new OutPrm(70, 20, MIDDLE_FONT, 2, false);
		setT->SetInputLink(0);	
		setT->SetVal("TEMP  ");
 //    
		CPanel* st = new TargetPanel(0.01, 0.1, 1.0);
		st->AddPrm(tt);
		st->AddPrm(tci);
		st->AddPrm(setT);
		st->AddPrm(tTimeC);
		st->SetMax(T_MAX);
		st->SetMin(T_MIN);				
		st->SetOutputFlag(&stecFl);
///<<<<<<<<<<<<<<<<<<<<TIME (C-MODE) SET PANEL>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>       
		OutPrm* timeTS = new OutPrm(0, 0, ARIALCYR28, 2, false, 0, "  ");
		timeTS->SetInputLink(&tmpTime);
		OutPrm* temperTS = new OutPrm(70, 0, MIDDLE_FONT, 2, false, 2,  "<C ");
		temperTS->SetInputLink(&tTg);
		OutPrm* iTS = new OutPrm(70, 10, MIDDLE_FONT, 2, false, 3, "A ");
		iTS->SetInputLink(&iTg);
		OutPrm* setTime = new OutPrm(70, 20, MIDDLE_FONT, 2, false);
		setTime->SetInputLink(0);	
		setTime->SetVal("TIME");
 //    
		CPanel* sTime = new TargetPanel(0.1, 1.0, 10.0);
		sTime->AddPrm(timeTS);
		sTime->AddPrm(temperTS);
		sTime->AddPrm(iTS);
		sTime->AddPrm(setTime);
		sTime->SetMax(TIME_MAX);
		sTime->SetMin(TIME_MIN);				
		sTime->SetOutputFlag(&stiFl);
//===============POWER SET PANEL ------------------------	
		OutPrm* tgPpow = new OutPrm(0, 0, ARIALCYR28, 2, false, 3);
		tgPpow->SetInputLink(&pTg);
		OutPrm* tgTpow = new OutPrm(70, 0, MIDDLE_FONT, 2, false, 2,  "<C ");
		tgTpow->SetInputLink(&tTg);
		OutPrm* tTimeP = new OutPrm(70, 10, MIDDLE_FONT, 2, false, 0, " S   ");
		tTimeP->SetInputLink(&tmpTime);
		OutPrm* setP = new OutPrm(70, 20, MIDDLE_FONT, 2, false);
		setP->SetInputLink(0);	
		setP->SetVal("POWER");
		
		CPanel* sPow = new TargetPanel(0.001, 0.1, 1.0);
		sPow->AddPrm(tgPpow);
		sPow->AddPrm(tgTpow);
		sPow->AddPrm(setP);
		sPow->AddPrm(tTimeP);
		sPow->AddPrm(tTimeC);
		sPow->SetMax(P_MAX);////////pMax);
		sPow->SetMin(P_MIN);////////pMin);
		sPow->SetOutputFlag(&spFl);
 //======+++++==TEMPERATURE (P-MODE) SET PANEL  +++++++++++++            
		OutPrm* ttPow = new OutPrm(0, 0, ARIALCYR28, 2, false, 2);
		ttPow ->SetInputLink(&tTg);
		OutPrm* tcpPow  = new OutPrm(70, 0, MIDDLE_FONT, 2, false, 3, "W ");
		tcpPow ->SetInputLink(&pTg);
		OutPrm* setTPow = new OutPrm(70, 20, MIDDLE_FONT, 2, false);//
		setTPow->SetInputLink(0);	
		setTPow->SetVal("TEMP   ");
		
		CPanel* stPow = new TargetPanel(0.01, 0.1, 1.0);
		stPow->AddPrm(ttPow );
		stPow->AddPrm(tcpPow );
		stPow->AddPrm(setTPow );
		stPow->AddPrm(tTimeC);
		stPow->SetMax(T_MAX);
		stPow->SetMin(T_MIN);
		stPow->SetOutputFlag(&stepFl);
		
///>>>>>>>>/////////////////\\\\\\\\\\\\\\\\\\\\>>>>>>>>>>>>>>>TIME (P-MODE) SET PANEL<<<////////\\\\\\\\\\\\\<<<<<<<<<<<<<<<<<<<<       
		OutPrm* timeTSP = new OutPrm(0, 0, ARIALCYR28, 2, false, 0, "  ");//);//, "S ");
		timeTSP->SetInputLink(&tmpTime);
		OutPrm* temperTSP = new OutPrm(70, 0, MIDDLE_FONT, 2, false, 1,  "<C ");
		temperTSP->SetInputLink(&tTg);
		OutPrm* pTSP = new OutPrm(70, 10, MIDDLE_FONT, 2, false, 1, "W ");
		pTSP->SetInputLink(&pTg);
//		OutPrm* setTime = new OutPrm(65, 20, MIDDLE_FONT, 2, false);
//		setTime->SetInputLink(0);	
//		setTime->SetVal("TIME SET");
 //    
		CPanel* sTimeP = new TargetPanel(0.1, 1.0, 10.0);
		sTimeP->AddPrm(timeTSP);
		sTimeP->AddPrm(temperTSP);
		sTimeP->AddPrm(pTSP);
		sTimeP->AddPrm(setTime);
		sTimeP->SetMax(TIME_MAX);
		sTimeP->SetMin(TIME_MIN);				
		sTimeP->SetOutputFlag(&stipFl);				
//==//=//=//=//=CALIBRATION PANEL  //++//+//+//+//+//+//+//+//+//+//++   
	    OutPrm* clC = new OutPrm(0, 0, ARIALCYR28, 2, false, 3);//, "A ");//ARIALCYR28, 1, false, 2, "6 ");
        clC->SetInputLink(&iSlowTg);//&curr);
        OutPrm* clT = new OutPrm(70, 0, MIDDLE_FONT, 2, false, 2, "<C ");
        clT->SetInputLink(&to);	
		OutPrm* clP = new OutPrm(70, 20, MIDDLE_FONT, 2, false, 3, "W ");
        clP->SetInputLink(&pClb);	
        OutPrm* clState = new OutPrm(70, 10, MIDDLE_FONT, 1, false);
        clState->SetInputLink(0);	
        clState->SetVal("CALIBRATE ");
		
        CnfClbrTgPanel* caP = new   CnfClbrTgPanel (clC, clP, clState, cnv, 0.001, 0.1, 1.0);
        caP->AddPrm(clC);
		caP->AddPrm(clT);//opTest2);
		caP->AddPrm(clP);
		caP->AddPrm(clState);
		caP->SetMax(P_MAX);//////////////////////////////pMax);
		caP->SetMin(P_MIN);////////////////////////////////pMin);
		//caP->SetOutputLink(&pClb);			
//		w->AddPrm(opWork);		

//======//////////=========/////\\\\\\\\\\\\\////////////ERASE CALIBATION PANEL
		double cpArg = 0, cpFun;
	    OutPrm* ecC = new OutPrm(0, 0, ARIALCYR28, 2, false, 3);//, "A ");//ARIALCYR28, 1, false, 2, "6 ");
        ecC->SetInputLink(&cpArg);//&curr);
        OutPrm* ecT = new OutPrm(70, 0, MIDDLE_FONT, 2, false, 2, "<C ");
        ecT->SetInputLink(&to);	
		OutPrm* ecP = new OutPrm(70, 20, MIDDLE_FONT, 2, false, 3, "W ");
        ecP->SetInputLink(&cpFun);	
        OutPrm* ecState = new OutPrm(70, 10, MIDDLE_FONT, 1, false);
        ecState->SetInputLink(0);	
        ecState->SetVal("XXXXX  ");
//		Converter* cnv = new Converter (0.5, 13.0, 9.0);
        CnfClbrTgPanel* ecPn = new  CnfClbrEraseTgPanel (ecC, ecP, ecState, cnv);
        ecPn->AddPrm(ecC);
		ecPn->AddPrm(ecT);//opTest2);
		ecPn->AddPrm(ecP);
		ecPn->AddPrm(ecState);
//		caP->SetMax(pMax);
//		caP->SetMin(pMin);

///////////=================TO MUCH CALIBRATION POINTS==========================
		OutPrm* ahtTMuch0 = new OutPrm(0, 0, MIDDLE_FONT, 2, false);
		ahtTMuch0->SetInputLink(0);
		ahtTMuch0->SetVal("TO MANY POINTS.");
		OutPrm* ahtTMuch1 = new OutPrm(0, 10, MIDDLE_FONT, 2, false);
		ahtTMuch1->SetInputLink(0);
		ahtTMuch1->SetVal("DELETE SOME");
		OutPrm* ahtTMuch2 = new OutPrm(0, 20, MIDDLE_FONT, 2, false);
		ahtTMuch2->SetInputLink(0);
		ahtTMuch2->SetVal("POINTS PLEASE.");

		CPanel*  ahtTM   = new AhtungPanel();
		ahtTM->AddPrm(ahtTMuch0);
		ahtTM->AddPrm(ahtTMuch1);
		ahtTM->AddPrm(ahtTMuch2);
//============CROSS PANEL NAVIGATION      
		w->SetNext(si);	
		w->SetOut(wP);
		w->SetCalibr(caP);
		w->SetErase(ecPn);
		w->SetOutputLink(&iSlowTg);
		si->SetNext(sTime);
		si->SetCalibr(si);
		st->SetNext(si);
		si->SetOut(w);
		st->SetOut(w);
		st->SetCalibr(st);
		si->SetOutputLink(&iTg);
		st->SetOutputLink(&tTg);	

		sTime->SetNext(st);
		sTime->SetOut(w);
		sTime->SetCalibr(sTime);
		sTime->SetOutputLink(&timeInput);
		
		wP->SetNext(sPow);
		wP->SetOut(w);
		wP->SetCalibr(wP);
		wP->SetErase(wP);
		wP->SetOutputLink(&p);
		sPow->SetNext( sTimeP);
		sPow->SetOut(wP);
		sPow->SetOutputLink(&pTg);
		sPow->SetCalibr(sPow);
		
		stPow->SetNext(sPow);
		stPow->SetOut(wP);
		stPow->SetOutputLink(&tTg);
		stPow->SetCalibr(stPow);
		
		sTimeP->SetNext(stPow);
		sTimeP->SetOut(wP);
		sTimeP->SetCalibr(sTimeP);
		sTimeP->SetOutputLink(&timeInput);
		
		caP->SetCalibr(caP);
		caP->SetNext(ahtTM);
		caP->SetOut(w);		
		ecPn->SetOut(w);

		ahtTM->SetOut(caP);
		ahtTM->SetNext(ecPn);
		
		CPanel* cuP = w;
/////////////////////////////////////////////////////////            
		unsigned short cDac= 0, okCnt = 0, ledCnt = 1;
		to = t->GetT();
		char buf[32];;
		while((to < 3.0 ) || (to > 50.0))
		{
			sprintf(buf, "ERR T %d   ", (int) to);
			displ->WriteStr(6, 10, buf, MIDDLE_FONT, 1,  false);
			SleepMs(200);			
			to = t->GetT();
		}
		trc.setUc(to);
		ClbrPoint*  point;
		while(1)
        {             		
            SleepUs(722);
//TEMPERATURE			
            if((++cDac % 20) == 0)
            {			
				tf = t->GetT();
				tf = ((double) abs(tf - to) > 2.5)? to : tf;
				to = trc.getUc(tf);
                tcDac->BuffWr(TControl(to, tTg));
            }
//current & time		
			if(stiFl || stipFl)
			{
				tmpTime =  timeInput * 10.0;
				if(dI >= 0.0005) 
				{
					curSpeedDiv = (int) (tmpTime / dI);					
					if(curSpeedDiv < SPEED_DIV_MIN)
					{		
						curSpeedDiv = SPEED_DIV_MIN;
						tmpTime = (double) SPEED_DIV_MIN * dI;
						timeInput = 0.1 * tmpTime;
					}
					else if(curSpeedDiv > SPEED_DIV_MAX)
					{		
						curSpeedDiv = SPEED_DIV_MAX;
						tmpTime = (double) SPEED_DIV_MAX * dI;
						timeInput = 0.1 * tmpTime;
					}
				}
			}
			else if(siFl  || spFl )
			{					
				dI = (double) abs (iTg - iSlowTg);				
				tmpTime = curSpeedDiv * dI;
				timeInput = 0.1 * tmpTime;
			}
			startFlPrev = tgIChStart;
			tgIChStart = !(siFl || stecFl || stiFl || spFl || stepFl || stipFl);
			if((cDac % curSpeedDiv) == 0)
			{
				if((double) abs(iTg - iTgPrev) > 0.0005)
				{
					pTg = cnv->Convert(iTg);
					pTgPrev = pTg;
					iTgPrev = iTg;
				}
				else if( (double) abs(pTg - pTgPrev) > 0.0005)
				{
					iTg = cnv->Reverse(pTg);
					if(iTg > I_MAX)
					{
						iTg = I_MAX;
						pTg = cnv->Convert(iTg);
					}
					else if(iTg < I_MIN)
					{
						iTg = I_MIN;
						pTg = cnv->Convert(iTg);
					}
					iTgPrev = iTg;
					pTgPrev = pTg;
				}						
				if(tgIChStart == true)
				{
					dI = (double) abs (iTg - iSlowTg);
					timeInput = 0.1 * curSpeedDiv * dI;
					bool pChange = false;
					if( (double) abs(p - prevP) > 0.0005)
					{
						pChange = true;
						iSlowTg = cnv->Reverse(p);
						pTg = pTgPrev = prevP = p;
					}
					if(iSlowTg == prevISlowTg)
						iSlowTg = SlowTg(iTg);//iTg;//
					else
						iTg = iSlowTg;
					prevISlowTg = iSlowTg;
					ldDac->BuffWr(ItoQ(iSlowTg));//iTg * 1000);//
					if(pChange == false)
						prevP = p = cnv->Convert(iSlowTg);	
					//led->SetOn(false);
				}
				else if((siFl == true) || (spFl == true))
				{
					//led->SetOn(true);
					dI = (double) abs (iTg - iSlowTg);
					timeInput = 0.1 * curSpeedDiv * dI;
				}					
			}
 //   		ldDac->BuffWr(iTg * 1000);//

////////////////////////t over
			led->SetOn(tOk);
			if((cDac % 5) == 0)
			{				
				if ( to  > T_ERR )
				{
					if( tOk == true )
					{
						prevWSpeedDiv = curSpeedDiv;
						curSpeedDiv = SPEED_DIV_AHTUNG;
						tOk = false;
					}					
					iTgErr = iTg - 0.003;
					if(iTgErr < 0.0)
						iTgErr = 0.0;
					iTg = iTgErr;
					cuP = w;
				}
				else if(to  < T_MAX)
				{	
					if( tOk == false )	
						curSpeedDiv = prevWSpeedDiv;
					if ( iTgErr  != iTg )					
						tOk = true;
				}
		////show		
				if(iSlowTg > 0.65)// lock & *
				{
					opWork->SetVal(">");					
//					if((iTg == iSlowTg) && ((double) abs (tTg - to) < 0.05))
//						opLock->SetVal("LOCK");
//					else if((iTg != iSlowTg) || ((double) abs (tTg - to) > 0.3) )
//						opLock->SetVal("     ");
				}
				else
//				{
					opWork->SetVal(" ");
//					opLock->SetVal("  ");
//				}
	            if(off == true)	//off	
				{
					unitAmp->SetVal("OFF   ");
					unitWatt->SetVal("OFF   ");
				}
				else if(tOk == false)//T OVER
				{
		//			opLock->SetVal("T OVER");
					unitAmp->SetVal("T OVER  ");
					unitWatt->SetVal("T OVER  ");
					if((++okCnt % 64) == 0)
						okLed->Toggle();
				}				
				else
				{
					unitAmp->SetVal("AMPERE ");
					unitWatt->SetVal("WATT  ");			
					okLed->SetOn(true);
	//                opErr->SetVal("TOV");//"      ");
				}
	//ERASE CALIBRATION PANEL
				point = cnv->Curr();
				if(point != 0)
				{
					cpArg = cnv->Curr()->x;
					cpFun = cnv->Curr()->y;			
				}
				else 
					cpArg = cpFun = 0.0;
				cuP = cuP->Service();
/////////////////////////////////////////////////////////////=============ON OFF===============//////////////////////////////////////////////////		
				if(bt->IsPressed() == false)
				{	            
					off = true;
					if((cDac % 3) == 0)
					{
						fpLeds[++ledCnt % 4]->SetOn(true);
					}
					else
						fpLeds[(ledCnt - 1) % 4]->SetOn(false);
					iTg = 0.0;
					tgIChStart = true;
					if(iSlowTg  == 0)
					{										   
						nPoints = 2*( cnv->GetNumPoints() +1) + 1;  //mem1 +1.0;
						flashBuf[0] = (double) nPoints;
						flashBuf[1] = tTg;
						flashBuf[2] = curSpeedDiv;
						ClbrPoint* cPnt = cnv->GetFirstPoint();
						for(int i = 3; i < nPoints; ++i)
						{						
							flashBuf[i]  = cPnt->x;						
							flashBuf[++i] = cPnt->y;
							cPnt = cPnt->next;
						}
						sv.save((unsigned long) &flashBuf, sizeof(flashBuf));
						SleepMs(500);
						while(1)
						{
							rl->SetOn(false);
							SleepMs(4000);
						}
					}	                   
				}
				else if (off == true)
				{        
					off = false;
					rl->SetOn(true);
					for(int i = 0; i < 4; ++i)
						fpLeds[i]->SetOn(true);
				}
			}
        }//The end of infinite while	
}//The end of main	


////		AbstractUART*  u = GetUART0();
//		char str[64];	
//		char lineFeed[3] = {10, 13, 0};
//		int nC;
	
		
//		IntervalGenerator(0, 60000000,  (unsigned long) T0Match);
//		TimerStart(0);



//bool WDTisTimeOut()
//{
//        return  ((WDMOD & 4) == 0)? false : true;
//}

//void WDTini (unsigned int time_in_ms)
//{
//        
//        unsigned int tics = time_in_ms  * 250 * 60;
//        WDMOD |= 3;
//        WDTC = tics;        
//        WDFEED = 0xaa;
//        WDFEED = 0x55;
//}

//void WDTrefresh()
//{
//      WDFEED = 0xaa;
//      WDFEED = 0x55;
//}




//      int uCnt = 0;
//       char  mess [] = { 'r',  'e',  'f', 'r',  'e',  's', 'h', ' ', ' ',  0, 10, 13};
//       char  hello [] = { 'h',  'e',  'l',  'l',  'o', '!', ' ',  0, 10, 13};
//       char oops[] = { 'o',  'o',  'p',  's',  '!', '!', '!', '!', ' ',  0, 10, 13};
//       char buf[32];
//       char inMess[32];
 //       tf ;//= 20.0;
 //      int nRein;
//       for(int i = 0; i < 30; i++)
//       {
//               SleepMs(500);
//               led->Toggle();        
//       }
//       if(WDTisTimeOut() == true)
//       {
//            ++nRein;
//           sprintf (buf, "%d", nRein);
//           strcat(buf, oops);
//       }
//       else
//       {
//           nRein = 0;
//           sprintf (buf, "%d", nRein);
//           strcat(buf, hello);
//       }
//       u->SendMessage(buf);
 //        WDTini(1000 * 240);

 //          SleepMs(100);
//             if( (++uCnt % 20) == 0 )
//             {
//                    u->Update();
//                    if(u->GetMessageSize() > 0)
//                    {
//                            WDTrefresh ();
//                            u->GetMessage(inMess);
//                            u->SendMessage(mess);    
//                    }
//             }
 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////				if((++nC % 1000) == 0)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////				{
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////					sprintf(str, " The_cycle_is ..._..._... %.d", nC/1000);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////					strcat(str, lineFeed);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////					u->SendMessage(str);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////					sprintf(str, " The_temperature_is ..._ ..._... %.2f", to);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////					strcat(str, lineFeed);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////					u->SendMessage(str);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////					
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////				}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////				else if ((nC % 20) == 0)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////				{
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////					u->SendMessage(lineFeed);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////					sprintf(str, " Q_W_E_R_T_Y_U_I_P_A_S_D_F_G_H_J_K_K_1234567890");
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////					strcat(str, lineFeed);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////					u->SendMessage(str);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	//				PTg(1, 31);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////				}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////				else
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////				{
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////					sprintf(str, ".");
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////					u->SendMessage(str);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////				}
		    //			dcnt += 1000;

