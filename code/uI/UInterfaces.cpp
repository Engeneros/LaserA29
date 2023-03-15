#include <UInterfaces.h>
#include <stdio.h>
#include <string.h>
#include <boarddevices.h>
#include <uc.h>

using namespace uController;
OutPrm::OutPrm (int x, int  y, const int font, int pitch, bool isInverted, unsigned char nDig)
{
	d = GetDisplay();
	SetPrm (x, y, font, pitch, isInverted, nDig);
	isSuff  = false;
}

OutPrm::OutPrm (int x, int  y, const int font, int pitch, bool isInverted, unsigned char nDig, char* suff )
{
	d = GetDisplay();
	SetPrm (x, y, font, pitch, isInverted, nDig);
	SetSuffix(suff);
}

void OutPrm::SetSuffix(char* str)
{
	int i;
	for( i = 0; ( (i < ( sizeof(sx) - 1) )  && (str[i]  != 0)); ++i )
		sx[i] = str[i];
	sx[i] = 0;
	isSuff = true;
}

void OutPrm::SetInputLink(double* inVal)
{
	iV = inVal;
}

void OutPrm::ChangeVal(double val)
{
	*iV = val;
}

double OutPrm::GetVal()
{
	return *iV;
}


OutPrm* OutPrm::Service()
{
	if(iV != 0)
	{
		
		if(nD == 3)
		{
			sprintf(oStr,   "%.3f", *iV);
		}
		else if(nD == 2)
		{
			sprintf(oStr,   "%.2f", *iV);
		}
		else if(nD == 1)
		{
			sprintf(oStr,   "%.1f", *iV);
		}
		else if(nD == 0)
		{
			sprintf(oStr,   "%d", (int) *iV);
		}
		if ( isSuff == true )
			strcat ( oStr, sx );
		else 
			strcat(oStr, " "); 
	}	
	Show();
	return next;
}


int OutPrm::Show ()
{
	return d->WriteStr(xo, yo, oStr, fo, po, inv);
}

void  OutPrm::SetNext (OutPrm* nx)
{ 			
	next = nx;
}

OutPrm* OutPrm::Next ()
{
	return next;
}

void OutPrm::SetVal (const char* str)
{
	char sym = 1;
	for(int i = 0; (i < 16) && (sym != 0); ++i)
	{				
		sym = oStr[i] = str[i];
	}
	oStr[15] = 0;
}

void OutPrm::Hide ()
{
	unsigned int len = strlen(oStr);
	len  = (len > 15)? 15 : len;
	for(int i = 0; i < len; ++len)
		oStr[i] = ' ';
	Show();
}

void OutPrm::SetPrm (int x, int  y, const int font, int pitch, bool isInverted, unsigned char n )
{
	xo = x;
	yo = y;
	fo = font;
	po = pitch;
	inv = isInverted;
	nD = n;
}

void OutPrm::SetX (int x)
{
	xo = x;
}

void OutPrm::SetY (int y)
{
	yo = y;
}

void OutPrm::SetFont (int fnt)
{
	fo = fnt;
}

void OutPrm::SetPitch (int pitch)
{
	po = pitch;
}

void OutPrm::SetInvertMode(bool isInverted)
{
	inv = isInverted;
}

void OutPrm::Invert()
{
	inv = !inv;
	Show();
}

CPanel::CPanel ()
{
	prms = cprm = 0;
	
	d = GetDisplay();
	enc = GetFrontPanelEncoder();
	eBt = GetFrontPanelEncoderButton ();

	upBt = GetUpButton (); 
	downBt = GetDownButton ();
	leftBt = GetLeftButton ();
	rightBt = GetRightButton ();
	okBt = GetOkButton ();

	upLed  = GetUpLED ();
	downLed = GetDownLED ();
	leftLed =  GetLeftLED ();
	rightLed = GetRightLED ();
	okLed = GetOkLED ();
	next = out = calibr = ers = 0;
}

void CPanel::Erase ()
{
	d->Erase ();
}

void CPanel::Show ()
{
	OutPrm* last = prms;
	while (last != 0)
	{
		last->Show();
		last = last->Next();
	}
}

void CPanel::SetNext (CPanel* nx)
{
	next = nx;
}

CPanel* CPanel::Next ()
{
	return next;
}

void CPanel::SetOutputLink(double* outVal)
{
	oV = outVal;
}

void CPanel::SetOutputFlag(bool* fl)
{
	oFlag = fl;
}

void CPanel::SetMax(const double val)
{
	max = val; 
}

void CPanel::SetMin(const double val)
{
	min = val;
}

void CPanel::SetOut (CPanel* pO)
{
	out = pO;
}

CPanel* CPanel::Out ()
{
	return out;
}

void CPanel::SetCalibr(CPanel* c)
{
	calibr = c;
}

CPanel* CPanel::Caliibr()
{
	return calibr;
}

void CPanel::SetErase(CPanel* c)
{
	ers = c;
}

CPanel* CPanel::GetErase()
{
	return ers;
}

void CPanel::AddPrm(OutPrm* p)
{
	if ( prms == 0 )
	{
		prms = cprm = p;
		cprm->SetNext ( 0 );
	}
	else
	{		
		cprm->SetNext ( p );
		cprm = p;
		cprm->SetNext ( 0 );
	}	
}

WorkPanel::WorkPanel() : CPanel()
{
	buttCnt = BUTT_DEAD_TIME;
	calibr = 0;
}

void WorkPanel::Conservate ()
{
	Erase();
	buttCnt = BUTT_DEAD_TIME;
}
	

CPanel* WorkPanel::Service()
{	
//	int tmp = enc->GetAccumulatedValue();
	static int clbrCnt = 0;
	static int eraseCnt = 0;
	OutPrm* last = prms;
	while (last != 0)
	{
		last->Service();
		last = last->Next();
	}
	CPanel* ret = this;
	if(--buttCnt <= 0)
	{
		if(okBt->IsPressed() == true)
		{
			ret = next;				
			Conservate();
		}
		else if (leftBt->IsPressed() == true) 
		{
			ret = out;				
			Conservate();
		}	
		else if (eBt->IsPressed() == true)//downBt->IsPressed() == true) 
			++clbrCnt;
		else if (upBt->IsPressed() == true)
			++eraseCnt;
		else
			eraseCnt = clbrCnt =  buttCnt = 0;
		if(clbrCnt > 100)
		{
			clbrCnt = 0;
			ret = calibr;				
			Conservate();					
		}	
		if(eraseCnt >100)
		{
			eraseCnt = 0;
			ret = ers;				
			Conservate();			
		}	
	}						
//	double  tmp = (double)(enc->GetAccumulatedValue());
	double tmp = *oV;	
	tmp += (double)(enc->GetAccumulatedValue()) * 0.001;
	if(tmp != *oV)
	{
		if(tmp > max)
			tmp = max;
		else if(tmp < min)
			tmp = min;
		*oV = tmp;				
	}			
	return ret;
}


TargetPanel::TargetPanel(double enFct , double lrFct, double udFct) : CPanel()
{
	cnt = SHOW_TIME;
	buttCnt = 0;
	ebCnt = EB_DEAD_TIME;
	factorEn = enFct;
	factorLR = lrFct;
	factorUD = udFct;
}

CPanel* TargetPanel::Service()
{			
	OutPrm* last = prms;
	while (last != 0)
	{
		last->Service();
		last = last->Next();
	}
	CPanel* ret;
	if(--cnt <=  0 )
	{
		cnt = SHOW_TIME;
		ebCnt = EB_DEAD_TIME;
		ret = out;
		Erase();
	}
	else
	{
		if(ebCnt <= 0)
		{
			if(okBt->IsPressed() == true)
			{
				ret = next;
				ebCnt = EB_DEAD_TIME;
				cnt = SHOW_TIME;
				Erase();
			}
			else
				ret = this;
		}
		else
		{
			--ebCnt;
			ret = this;		
		}		
		double tmp = *oV;	
		if(--buttCnt <= 0)
		{
			buttCnt = BUTT_DEAD_TIME;
			if(downBt->IsPressed() == true)			
				tmp -= factorUD;
			else if(leftBt->IsPressed() == true)
				tmp -= factorLR;
			else if(rightBt->IsPressed() == true)
				tmp += factorLR;
			else if(upBt->IsPressed() == true)				
				tmp += factorUD;
			else
				buttCnt = 0;
		}						
		tmp += (double)(enc->GetAccumulatedValue()) * factorEn;
		if(tmp != *oV)
		{
			cnt = SHOW_TIME;
			if(tmp > max)
				tmp = max;
			else if(tmp < min)
				tmp = min;
			*oV = tmp;				
		}			
	}		
	*oFlag = (ret == this)? true : false;
	return ret;
}

AhtungPanel::AhtungPanel() : TargetPanel()
{
	cnt = SHOW_TIME;
	buttCnt = BUTT_DEAD_TIME;
}
void AhtungPanel::Conservate()
{
		cnt = SHOW_TIME;
		buttCnt = BUTT_DEAD_TIME;
		Erase();
}

CPanel* AhtungPanel::Service()
{
	OutPrm* last = prms;
	while (last != 0)
	{
		last->Service();
		last = last->Next();
	}
	CPanel* ret = this;
	if(--cnt <=  0 )
	{
		ret = out;
		Conservate();
	}
	if(--buttCnt <= 0)
	{
		buttCnt = BUTT_DEAD_TIME;
		if(upBt->IsPressed() == true)			
		{
			ret = next;
			Conservate();
		}	
		else if(okBt->IsPressed() == true)
		{
			ret = out;
			Conservate();
		}
		else
			buttCnt = 0;
	}						
	return ret;	
}

///////////////////////
CnfClbrTgPanel::CnfClbrTgPanel(OutPrm* a, OutPrm* f, OutPrm* ind, AbstractConverter* c, double enFct, double lrFct, double udFct) : TargetPanel(enFct, lrFct, udFct)
{
	arg = a;
	chngPrm = f;
	chngShow = ind;
	con = c;
	isFirst = true;
	buttCnt = 2 * BUTT_DEAD_TIME;
}

void CnfClbrTgPanel::Conservate()
{
		cnt = SHOW_TIME;
		ebCnt = EB_DEAD_TIME;
		buttCnt = 2 * BUTT_DEAD_TIME;	
		Erase();
		isFirst = true;
}

CPanel* CnfClbrTgPanel::Service()
{		
	if(isFirst == true)
	{
		isFirst = false;
		chngPrm->ChangeVal(con->Convert(arg->GetVal()) );
		chngShow->SetVal("CALIBRATE");
	}
	OutPrm* last = prms;
	while (last != 0)
	{
		last->Service();
		last = last->Next();
	}
	CPanel* ret = this;
	if(--cnt <=  0 )
	{
		ret = out;
		Conservate();
	}
	else
	{
		if(ebCnt <= 0)
		{
			if(eBt->IsPressed() == true)//			if(okBt->IsPressed() == true)
			{
				ret = out;
				Conservate();
			}
			else
				ret = this;
		}
		else
		{
			--ebCnt;
			ret = this;		
		}		
		double prevV = chngPrm->GetVal();//*oV;	
		double tmp = prevV;
		if(buttCnt <= 0)
		{
//			buttCnt = BUTT_DEAD_TIME;
//			if(downBt->IsPressed() == true)			
//				tmp -= factorUD;
//			else if(leftBt->IsPressed() == true)
//				tmp -= factorLR;
//			else if(rightBt->IsPressed() == true)
//				tmp += factorLR;
//			else if(upBt->IsPressed() == true)				
//				tmp += factorUD;
//			else
//				buttCnt = 0;
//		}						
//		tmp += (double)(enc->GetAccumulatedValue()) * factorEn;
			
			
			buttCnt = BUTT_DEAD_TIME;
			if(downBt->IsPressed() == true)			
				tmp -= factorUD;
			else if(leftBt->IsPressed() == true)
				tmp -= factorLR;
			else if(rightBt->IsPressed() == true)
				tmp += factorLR;
			else if(upBt->IsPressed() == true)				
				tmp += factorUD;
			else if(okBt->IsPressed() == true)				
			{
//				*oV = prevV;
				double xo = arg->GetVal() ;
				con->AddPoint(xo, prevV);
				if(con->GetNumPoints() > 	MAX_CLBR_POINTS)
				{
					ClbrPoint* tmpP = con->GetPoint(xo);
					if(tmpP != 0)
						con->DeletePoint(tmpP);
					ret = next;
					Conservate();
				}
				else
					chngShow->SetVal("WRITED   ");
			}
			else
				buttCnt = 0;
		}						
		else
			--buttCnt;
		tmp += (double)(enc->GetAccumulatedValue()) * factorEn;
		if(tmp != prevV)//*oV)
		{
			chngShow->SetVal("CALIBRATE");
			cnt = SHOW_TIME;
			if(tmp > max)
				tmp = max;
			else if(tmp < min)
				tmp = min;
			chngPrm->ChangeVal(tmp);//*oV = tmp;				
		}		
	}			
	return ret;
}

CnfClbrEraseTgPanel::CnfClbrEraseTgPanel ( OutPrm* a,  OutPrm* f, OutPrm* ind, AbstractConverter* c) : CnfClbrTgPanel(a,  f,  ind,  c)
{
	isFirst = true;
	delCnt = 0;
}

CPanel* CnfClbrEraseTgPanel::Service()
{		
	if(isFirst == true)
	{
		isFirst = false;
		chngShow->SetVal("ERASE  ");
		cPoint = con->GetFirstPoint();
		con->SetCurr(cPoint);
	}
	OutPrm* last = prms;
	while (last != 0)
	{
		last->Service();
		last = last->Next();
	}
	CPanel* ret = this; // TIMEOUT 
	if((--cnt <=  0) || (cPoint == 0) )
	{
		delCnt = 0;
		Conservate();
		ret = out;		
	}
	ClbrPoint* t;
	if(buttCnt <= 0)// BUTTONS
	{
		buttCnt = BUTT_DEAD_TIME;
		if(downBt->IsPressed() == true)
		{
			cPoint = con->GetFirstPoint();
			con->SetCurr(cPoint);
			cnt = SHOW_TIME;
		}
		if(leftBt->IsPressed() == true)
		{
			t = con->Prev(cPoint);
			if(t != 0)
				cPoint = t;
			con->SetCurr(cPoint);
			cnt = SHOW_TIME;
		}
		else if(rightBt->IsPressed() == true)
		{
			t = con->Next(cPoint);
			if(t != 0)
				cPoint = t;
			con->SetCurr(cPoint);
			cnt = SHOW_TIME;
		}
		else if(upBt->IsPressed() == true)	
		{
			cPoint = con->GetLastPoint();
			con->SetCurr(cPoint);
			cnt = SHOW_TIME;
		}
		else if(okBt->IsPressed() == true)				
		{
			delCnt = DELAY_DEAD_TIME;
			cnt = SHOW_TIME;
		}
		else
			buttCnt = 0;
	}						
	else
		--buttCnt;				
	if(--delCnt < 0)
		chngShow->SetVal("ERASE  ");
	else if (delCnt == 0)
	{
			con->DeletePoint(cPoint);
			cPoint =con->Curr();
	}
	else
		chngShow->SetVal("DELETED");
	return ret;
}


