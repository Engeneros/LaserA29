#ifndef U_INTERFACE_S_H
#define U_INTERFACE_S_H
#include <AbstractDisplay.h>
#include <AbstractLED.h>
#include <AbstractButton.h>
#include <AbstractEncoder.h>

using namespace Board;

class OutPrm
{
public:
	OutPrm (int x, int  y, const int font, int pitch, bool isInverted, unsigned char nDig = 2);
	OutPrm (int x, int  y, const int font, int pitch, bool isInverted, unsigned char nDig , char* suff );
	void SetNext (OutPrm* nx);
	OutPrm* Next ();
	void SetVal (const char* str);
	void SetInputLink(double* inVal);
	void ChangeVal(double val);
	double GetVal();
	int Show ();
	void Hide ();
	void SetPrm ( int x, int  y, const int font, int pitch, bool isInverted, unsigned char n );
	void SetSuffix(char* str);
	void SetX (int x);
	void SetY (int y);
	void SetFont (int fnt);
	void SetPitch (int pitch);
	void SetInvertMode (bool isInverted);
	void Invert ();
	virtual OutPrm* Service();
protected:
	OutPrm* next;
	char oStr [16];
	char sx[4];
	int xo, yo, fo, po;
	bool inv, isSuff;	
	AbstractDisplay* d;
double* iV;
	unsigned char nD;
};

class CPanel
{
public:
	CPanel ();
	void Erase ();
	void Show ();
//	virtual bool Inputing(unsigned long& butt, int& enc);
	void SetNext (CPanel* nx);
	CPanel* Next ();
	void SetOut (CPanel* pO);
	CPanel* Out ();
	 void SetCalibr(CPanel* c);
	CPanel* Caliibr();
	void SetErase(CPanel* c);
	CPanel* GetErase();
	void SetOutputLink(double* outVal);
	void SetOutputFlag(bool* fl);
	void SetMax(const double val);
	void SetMin(const double val);

	void AddPrm(OutPrm* p);
	virtual CPanel* Service() = 0;
protected:
	double* oV;
	double max, min;
	bool* oFlag;
	OutPrm* prms;
	OutPrm* cprm;
	CPanel* next;
	CPanel* out;
	CPanel* calibr;
	CPanel* ers;

	AbstractDisplay* d;
	AbstractEncoder* enc;
	AbstractButton* eBt;

	AbstractButton* upBt;
	AbstractButton* downBt;
	AbstractButton* leftBt;
	AbstractButton* rightBt;
	AbstractButton* okBt;

	AbstractLED* upLed;
	AbstractLED* downLed;
	AbstractLED* leftLed;
	AbstractLED* rightLed;
	AbstractLED* okLed;
};

class WorkPanel : public  CPanel
{
public:
	WorkPanel();
	virtual CPanel* Service() ;
protected:
	virtual void Conservate();
	static const int BUTT_DEAD_TIME = 20;
	int buttCnt;
};

class TargetPanel : public  CPanel
{
public:
	TargetPanel(double enFct = 0.1, double lrFct = 1.0, double udFct = 10000.0 );
	virtual  CPanel* Service() ;
protected:
	static const int SHOW_TIME = 200;
	static const int BUTT_DEAD_TIME = 20;
	static const int EB_DEAD_TIME = 20;
	int cnt, buttCnt, ebCnt;
	double factorEn, factorLR, factorUD;
};

class AhtungPanel : public  TargetPanel
{
public:
	AhtungPanel();
	virtual CPanel* Service() ;
protected:
	virtual void Conservate();
};

#include <AbstractConverter.h>
const unsigned int MAX_CLBR_POINTS = 100;	

class CnfClbrTgPanel : public  TargetPanel
{
public:
	CnfClbrTgPanel ( OutPrm* a,  OutPrm* f, OutPrm* ind, AbstractConverter* c, double enFct = 0.1, double lrFct = 1.0, double udFct = 10000.0 );
	virtual  CPanel* Service() ;
protected:
	AbstractConverter* con;
	bool isFirst;
	OutPrm* arg;
	OutPrm* chngPrm;
	OutPrm* chngShow;
	virtual void Conservate();
};

class CnfClbrEraseTgPanel  : public  CnfClbrTgPanel
{
public:
	CnfClbrEraseTgPanel ( OutPrm* a,  OutPrm* f, OutPrm* ind, AbstractConverter* c);
	virtual  CPanel* Service() ;
protected:
	ClbrPoint* cPoint;
	static const int DELAY_DEAD_TIME = 16;
	int delCnt;
};


#endif



