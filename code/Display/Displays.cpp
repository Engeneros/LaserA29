#include <Displays.h>
#include <Delay.h>
#include <uc.h>
#include <LPC2148_ex.h>
#include <Displays.h>
#include <fonts_0D.h>


using namespace Board;
using namespace uController;
using namespace HwSleep;

void EA122::wr(unsigned char msb, unsigned char lsb)
{ 
	unsigned long  port1 = ((unsigned long) msb) << 10; 
	port1 |= lsb;
	port1 <<= 16;
	IOSET1 = (port1 & 0x1fff0000);
	IOCLR1 = ~port1 & 0x1fff0000;
}   
            
void inline EA122::clkLeft(void){
     IOCLR1 = 0x01000000;
	 IOSET1 = 0x01000000;
//	 DelayUs(1);
	 IOCLR1 = 0x01000000;
}    

void inline EA122::clkRight(void){
     IOCLR1 = 0x02000000;
	 IOSET1 = 0x02000000;
//	 DelayUs(1);
	 IOCLR1 = 0x02000000;
}   


void EA122::on(unsigned char isOn){

     if(isOn)
         wr(4, 0xaf);                     
     else  
        wr(4, 0xae);                    
     clkLeft();  
     clkRight();
}  


void EA122::setPageLeft(unsigned char page){  
     wr(4, 0xb8 | (page &  3));
     clkLeft();  
}           

void EA122::setPageRight(unsigned char page){
     wr(4, 0xb8 | (page &  3));
     clkRight();
}           


void EA122::setTopLine(unsigned char line){
     wr(4, 0xc0 | (line &  0x1f));
     clkLeft();
     clkRight();   
}

void EA122::setColumn(unsigned char column){    
     if(column < 61){
        wr(4, column);
        clkLeft();    
     }   
     else if(column < 122){
        wr(4, column - 61);       
        clkRight();        
     }   
}

void EA122::dataWr(unsigned char x, unsigned char y, unsigned char data){
	x = (x < DISPLAY_LENGH) ? x : DISPLAY_LENGH - 1;
     if (dD.bl[x][y] != data)
	 {
	 	if(x < 61)
        	setPageLeft(y);
     	else// if(x < 122)
		{
        	setPageRight(y);     		
		}
     	setColumn(x);
     	wr(6, data);   
     	if(x < 61)
        	 clkLeft();
     	else //if(x < 122)
        	clkRight();  
		dD.bl[x][y] = data;
	 }        
}
     

void EA122::Erase(void){   
unsigned char x, y; 

    for(y = 0; y < 4 ; y++)
	{
         for(x = 0; x < 122  ; x++){
             dataWr(x, y, 0);     
        }
    }

} 

void EA122::hardErase(void){   
unsigned char x, y; 
   for(y = 0; y < 4 ; y++){
         for(x = 0; x < 122  ; x++){
             dataWr(x, y, 0xff);     
        }
    }
	Erase();
}  

void EA122::reset(void)
{
      wr(0, 0);
      wr(4, 0);  
}

void EA122::displayIni(void){
   	//port12GPIO();
	PCfg(1, 16, 0);
	PCfg(1, 17, 0);
	PCfg(1, 18, 0);
	PCfg(1, 19, 0);

	PCfg(1, 20, 0);
	PCfg(1, 21, 0);
	PCfg(1, 22, 0);
	PCfg(1, 23, 0);
	
	PCfg(1, 24, 0);
	PCfg(1, 25, 0);
	PCfg(1, 26, 0);
	PCfg(1, 27, 0);
	
	PCfg(1, 28, 0);
// 	PCfg(1, 29, 0);
// 	PCfg(1, 30, 0);
// 	PCfg(1, 31, 0);

	PDir(1, 16, true);
	PDir(1, 17, true);
	PDir(1, 18, true);
	PDir(1, 19, true);

	PDir(1, 20, true);
	PDir(1, 21, true);
	PDir(1, 22, true);
	PDir(1, 23, true);
	
	PDir(1, 24, true);
	PDir(1, 25, true);
	PDir(1, 26, true);
	PDir(1, 27, true);
	
	PDir(1, 28, true);
// 	PDir(1, 29, false);
// 	PDir(1, 30, true);
// 	PDir(1, 31, true);



	
	IODIR1 |= 0x1fff0000;
    reset();
//    DelayMs(10);//---------------                          
    on(1);      
//////	DelayMs(10);
//////    DelayMs(10);
    setPageLeft(0);  
//////    DelayMs(10);  
    setPageRight(0);
    setColumn(0);
    setTopLine(0); 
//////    DelayMs(10);
    hardErase();
    
}
EA122::	EA122(void)
{
	unsigned char x;
		displayIni();
	for(x = 0; x < DISPLAY_LENGH; x++)
		dD.dw[x] = 0;
	refXb = refXe = refYb = refY = 0;
}

EA122::~EA122()
{

}

char EA122::eraseMtx(unsigned char xb, xEMaskY coord, bool isInverted)
{
	char cX = xb, cY;//current x, y for output 
	bsl col; //column for output
	if(coord.xe >= DISPLAY_LENGH)
		coord.xe = DISPLAY_LENGH - 1;
	unsigned long border = 	(coord.msk >> 1) | (coord.msk << 1);	
	if(xb <= coord.xe){
		for(cX = xb; cX < coord.xe; cX++){
			if(isInverted == false)	
				col.dw = dD.dw[cX] & ~border;//coord.msk;
			else
				col.dw = dD.dw[cX] | border;
			for(cY = coord.lb; cY < coord.le; cY++)
				dataWr(cX, cY, col.b[cY]);							
		}
	}		
	return cX;	
}


xEMaskY EA122::writeMtx(unsigned char x, unsigned char  y, const unsigned char* mtx, bool isInverted)
{
//x,y coordinate of left and top angle of output rectangle
	unsigned char szX, szY;// sizes in pixels (real Xsize = szX -2 ) ); 
    unsigned char nLine ;//Y size in byte	
	char cX, cY, cNP = 2;//current x, y fo output and curent column in array
	xEMaskY  ret;
	unsigned char lineB;//, yShift;//first line num & Yshift in pixels vs line begin
	bsl col; //column for output

	szY = *(mtx);
	szX = *(mtx	+ 1);
	nLine = (szY + 7) >> 3;
								
	lineB = y >> 3;								
	ret.msk = 0xffffffff;
	ret.msk >>= 32 - szY;
	ret.msk <<=	y;
	unsigned long border = 	(ret.msk | (ret.msk >> 1)) ^ ( ret.msk | (ret.msk << 1));
	ret.lb = lineB;
	ret.le = lineB + nLine;
	ret.xe = x;

	if( y < (LINES << 3)) //(x < DISPLAY_LENGH) &&   (y < (LINES << 3)))
	{
		for(cX = x, cNP = 2; (cNP < szX) && (cX < DISPLAY_LENGH); cX++, cNP++)
		{
			col.dw = 0;
			for(cY = nLine; cY > 0; cY--){
				col.dw <<= 8;
				if(isInverted == false)
				{
					col.dw |= *(mtx + ((cY - 1) * szX + cNP));											
  				}
				else
				{
					col.dw |= (~(*(mtx + ((cY - 1) * szX + cNP)))) & 0xff;
				}
			}				
			col.dw  <<= y;
			if(isInverted)
			{
				col.dw &= ret.msk ;//| (ret.msk >> 1) | (ret.msk << 1);
				col.dw |=  border;//(ret.msk | (ret.msk >> 1)) ^	( ret.msk | (ret.msk << 1));
			}
			col.dw |= (dD.dw[cX] & ~ret.msk);
			if(isInverted == false)
				col.dw &= ~border;
			for(cY = lineB; cY < ret.le ; cY++)	
				dataWr(cX, cY, col.b[cY]);
		}
		ret.xe = cX;		
	}
	return ret;	
}


#include <fonts_0D.h>

int EA122::WriteStr(int x, int  y,  const char* str, const int font, int pitch, bool isInverted)
{
	xEMaskY  ret;

	ret.xe = x;
	while(*str){		
			ret = writeMtx(x, y, 
		           *((const unsigned char**)FONT_ADDR0[font] +  symAddrShift[font][*str++]), isInverted);
		x = ret.xe;
		ret.xe +=  pitch;
		eraseMtx(x, ret, isInverted);
		x = ret.xe;				
	}
	return ret.xe;
}


