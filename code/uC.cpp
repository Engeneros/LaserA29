#include <uC.h>

using namespace uController;

Led::Led(unsigned char portGroup, unsigned char portNum)
{
	group = portGroup;
	num = portNum;
	PCfg(group, num, 0);
	PDir(group, num, true);
	PLow(group, num);
}
void Led::Toggle(void)
{
	PTg(group, num);
}
void Led::High(void)
{
	PHigh(group, num);
}			
void Led::Low(void)
{
	PLow(group, num);
}
void Led::Off(void)
{
	PDir(group, num, false);
} 		  
Led::~Led(void)
{
	Off();
} 
////////                   LED===    

/////////////           == LPC2478

#ifdef LPC2478
#include <lpc24xx_ex.h>
#endif
#ifdef LPC2148
#include <LPC2148_ex.h>
#endif
////////////////               == Port
//
void uController::PCfg(unsigned char portNumM, unsigned char portNumL, unsigned char func)
{
	(*(volatile unsigned long *)(PINSEL_BASE_ADDR + (portNumM << 3) + ((portNumL >> 4) << 2))) &=  ~(3 << ((portNumL & 0xf) << 1));
	(*(volatile unsigned long *)(PINSEL_BASE_ADDR + (portNumM << 3) + ((portNumL >> 4) << 2))) |=  ((func & 3) << ((portNumL & 0xf) << 1));
}

void  uController::PDir(unsigned char portNumM, unsigned char portNumL, bool isOut)
{
	if(portNumM < 2)
	{
		if(isOut)
			(*(volatile unsigned long *)(GPIO_BASE_ADDR + 8 + (0x10 * portNumM) )) |=  1 << portNumL ;
		else
			(*(volatile unsigned long *)(GPIO_BASE_ADDR + 8 + (0x10 * portNumM) )) &=  ~(1 << portNumL) ;
	}
	else
	{	
		if(isOut)
			(*(volatile unsigned long *)(FIO_BASE_ADDR + (0x20 * portNumM) )) |=  1 << portNumL ;
		else
			(*(volatile unsigned long *)(FIO_BASE_ADDR + (0x20 * portNumM) )) &=  ~(1 << portNumL) ;
	}
}

void  uController::PHigh(unsigned char portNumM, unsigned char portNumL)		
{
	if(portNumM < 2)
		(*(volatile unsigned long *)(GPIO_BASE_ADDR + 4  + (0x10 * portNumM) )) =  1 << portNumL ;
	else
		(*(volatile unsigned long *)(FIO_BASE_ADDR + 0x18 + (0x20 * portNumM) )) =  1 << portNumL ;

}

void  uController::PLow(unsigned char portNumM, unsigned char portNumL)		
{
	if(portNumM < 2)
		(*(volatile unsigned long *)(GPIO_BASE_ADDR + 0xc  + (0x10 * portNumM) )) =  1 << portNumL ;
	else
		(*(volatile unsigned long *)(FIO_BASE_ADDR + 0x1c  + (0x20 * portNumM) )) =  1 << portNumL ;

}

void  uController::PTg(unsigned char portNumM, unsigned char portNumL)		
{
	if(portNumM < 2)
		(*(volatile unsigned long *)(GPIO_BASE_ADDR + (0x10 * portNumM) )) ^=  1 << portNumL ;
	else
		(*(volatile unsigned long *)(FIO_BASE_ADDR + 0x14 + (0x20 * portNumM) )) ^=  1 << portNumL ;

}


bool  uController::PGet(unsigned char portNumM, unsigned char portNumL)		
{
	if(portNumM < 2)
		return (*(volatile unsigned long *)(GPIO_BASE_ADDR + (0x10 * portNumM) )) &  (1 << portNumL) ;
	else
		return (*(volatile unsigned long *)(FIO_BASE_ADDR + 0x14 + (0x20 * portNumM) )) & (1 << portNumL);

}



//////////////               Port ==

// //////////////////////////// == I2C ==
#ifdef LPC2478


static const unsigned long I2C_BASE[] = {I2C0_BASE_ADDR, I2C1_BASE_ADDR, I2C2_BASE_ADDR};
void uController::I2cIni(unsigned char ifcNum)
{
	switch(ifcNum)
	{
		case 0 :
		{
			PCfg(0, 27, 1);//sda0
			PCfg(0, 28, 1);//scl0	
			I20CONCLR = 0x7c;
			I20SCLH   = 1000;//(((CRYSTAL_FREQUENCY * PLL_FACTOR) / VPBDIV_FACTOR) / i2cFrequency + 1) / 2;1
			I20SCLL   = 1000;//(((CRYSTAL_FREQUENCY * PLL_FACTOR) / VPBDIV_FACTOR) / i2cFrequency) / 2;
			//reset registers
			I20CONSET = 0x40;
		}
		break;
		case 1 :
			{
				PCfg(2, 14, 3);// sda1
				PCfg(2, 15, 3);// scl1
				I21CONCLR = 0x7c;
				I21SCLH   = 1000;//(((CRYSTAL_FREQUENCY * PLL_FACTOR) / VPBDIV_FACTOR) / i2cFrequency + 1) / 2;1
				I21SCLL   = 1000;//(((CRYSTAL_FREQUENCY * PLL_FACTOR) / VPBDIV_FACTOR) / i2cFrequency) / 2;
				//reset registers
				I21CONSET = 0x40;

			}
			break;
		case 2:
			{
			PCfg(0, 10, 2);// sda2
			PCfg(0, 11, 2);// scl2
			I22CONCLR = 0x7c;
			I22SCLH   = 1000;//(((CRYSTAL_FREQUENCY * PLL_FACTOR) / VPBDIV_FACTOR) / i2cFrequency + 1) / 2;1
			I22SCLL   = 1000;//(((CRYSTAL_FREQUENCY * PLL_FACTOR) / VPBDIV_FACTOR) / i2cFrequency) / 2;
			//reset registers
			I22CONSET = 0x40;
			}
			break;
		default:
			{
			
			}
			break;
	}
}

unsigned char  uController::I2cStart(unsigned char ifcNum)
{
	(*(volatile unsigned long *)(I2C_BASE[ifcNum] ))   = 0x20;   //STA = 1, set start flag
	while((*(volatile unsigned long *)(I2C_BASE[ifcNum] ) & 8)  == 0)
	{		
	}
	unsigned char ret = *(volatile unsigned long *)(I2C_BASE[ifcNum] + 4 ) ;		
	(*(volatile unsigned long *)(I2C_BASE[ifcNum] + 0x18 )) = 0x20;
	return ret;
}

unsigned char  uController::I2cStopWr (unsigned char ifcNum)
{
	(*(volatile unsigned long *)(I2C_BASE[ifcNum] )) = 0x10;
	(*(volatile unsigned long *)(I2C_BASE[ifcNum] + 0x18 )) = 8;
	return 0;
}

unsigned char  uController::I2cStopRd ( unsigned char ifcNum)
{
	(*(volatile unsigned long *)(I2C_BASE[ifcNum] )) = 0x10;
	(*(volatile unsigned long *)(I2C_BASE[ifcNum] + 0x18 )) = 0xc;
	return 0;
}

unsigned char  uController::I2cRawRead (unsigned char ifcNum, unsigned char &byte)
{
	(*(volatile unsigned long *)(I2C_BASE[ifcNum] + 0x18 )) = 0xc;//I2CxCONCLR
	while(((*(volatile unsigned long *)(I2C_BASE[ifcNum] )) & 8)  == 0)//I2CxCONSET
	{
	}
	byte = (*(volatile unsigned long *)(I2C_BASE[ifcNum] + 8 ));//I2CxDAT
	return  *(volatile unsigned long *)(I2C_BASE[ifcNum] + 4 );//I2CxSTAT

}

unsigned char  uController::I2cRawWrite (unsigned char ifcNum, unsigned char byte)
{
	(*(volatile unsigned long *)(I2C_BASE[ifcNum] + 8 )) = byte;
	(*(volatile unsigned long *)(I2C_BASE[ifcNum] )) = 4;
	(*(volatile unsigned long *)(I2C_BASE[ifcNum] + 0x18 )) = 8;
	while(((*(volatile unsigned long *)(I2C_BASE[ifcNum] )) & 8) == 0)
	{
	}
	return  *(volatile unsigned long *)(I2C_BASE[ifcNum] + 4 );	
}

int  uController::I2cGetByte(unsigned char ifcNum, unsigned char dev, unsigned char addr, unsigned char &data)
{	
	//	char str[256];
//	unsigned char st[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; 
	unsigned char status = I2cStart(ifcNum);	
//	st[0] = status;
	if(status == 8)
	{
//		st [1] = 
		status = I2cRawWrite (ifcNum, dev << 1);
		if(status == 0x18)
		{
//			st [2] = 
			status = I2cRawWrite (ifcNum, addr);
			if(status == 0x28)
			{
				(*(volatile unsigned long *)(I2C_BASE[ifcNum] + 0x18 )) = 8;	
//				st [3] = 
				status = I2cStart(ifcNum);	
				if(status == 0x10)
				{
//					st [4] = 
					status = I2cRawWrite (ifcNum, (dev << 1) | 1);
					if(status == 0x40)
					{
//						st [5] = 
						status = I2cRawRead (ifcNum, data);
//						st[6] = data;
					}

				}
			}	
		}

	}
	I2cStopRd (ifcNum);
	
	unsigned char neededToReadRegister = I2C_BASE[ifcNum];
//	st[7] =  *(volatile unsigned long *)(I2C_BASE[ifcNum] + 4 );
	//	sprintf(str, "Read: %d, %d, %d,   %d,  %d, %d, %d, %d", st[0], st[1], st[2], st[3], st[4], st[5], st[6], st[7]);
	//	uartSendStrE(str);
//	delay_ms(400);
	return status; 
}

int  uController::I2cSendByte(unsigned char ifcNum, unsigned char dev, unsigned char addr, unsigned char data)
{
	unsigned char status = I2cStart(ifcNum);	
	if(status == 8)
	{
		status = I2cRawWrite (ifcNum, dev << 1);
		if(status == 0x18)
		{
			status = I2cRawWrite (ifcNum, addr);
			if(status == 0x28)
			{
				status = I2cRawWrite (ifcNum, data);

			}	
		}

	}
	I2cStopWr (ifcNum);
	return status; 
}
#endif
// /////   == SSP 

/////   == SSP 
static const unsigned long SSP_BASE[] = {SSP0_BASE_ADDR, SSP1_BASE_ADDR};
void  uController::SSPPinCfg(unsigned char ifcNum)
{
#ifdef LPC2478	
	switch(ifcNum)
	{
		case 0:
		{
			PCfg(0, 15, 3);//sclk 0
			PCfg(0, 17, 3);//miso 0
			PCfg(0, 18, 3);//mosi 0	
			
		}
		break;
		case 1:
		{
			PCfg(4, 20, 3);//sclk 1
			PCfg(4, 22, 3);//miso 1
			PCfg(4, 23, 3);//mosi 1
		}
		default:
		{
		}
		break;
	}
#endif	
}

void   uController::SSPSetDataSize(unsigned char ifcNum, unsigned char n)
{
	(*(volatile unsigned long *)(SSP_BASE[ifcNum]  ))  &= ~0xf;//SSPxCR0
	(*(volatile unsigned long *)(SSP_BASE[ifcNum]  )) |= (n - 1);
}
void   SSPSetClkDiv(unsigned char ifcNum, unsigned char div)
{
	(*(volatile unsigned long *)(SSP_BASE[ifcNum] + 0x10 ))	 = div;//  SSPxCPSR clock prescaler;
}

void   uController::SSPSetfronts(unsigned char ifcNum, unsigned char f)
{
	(*(volatile unsigned long *)(SSP_BASE[ifcNum]  )) &= ~(3 << 6) ;// SSPxCR0  clock prescaler;
	(*(volatile unsigned long *)(SSP_BASE[ifcNum]  )) |= 	(f << 6) ;
}

void   uController::SSPEnable(unsigned char ifcNum)
{
#ifdef LPC2478	
	switch(ifcNum)
	{
	case 0:
		{
			PCONP |= 1 << 21;
			SSP0CR1 = 2;//enable spi// clock prescaler;
			PCLKSEL1	&= ~(3 << 10);
			PCLKSEL1 |= 1 << 10;
		}
		break;
	case 1:
		{
			PCONP |= 1 << 10;
			SSP1CR1 = 2;//enable spi// clock prescaler;
	//		PCLKSEL0	&= ~(3 << 20);
		//	PCLKSEL0 |= 1 << 20;
		}
	default:
		{
		}
		break;
	}

#endif
}

void SSPEnableInterrups(unsigned char ifcNum, unsigned long isrAddr)
{
#ifdef LPC2478
	switch(ifcNum)
	{	
		case 0 : 
		{			
			VICVectAddr10 = isrAddr;
		    VICVectCntl10 = 0x20 | 10;     // use it for SSP0 Interrupt
		    VICIntEnable |= 1 << 10;      //Enable SSP0 Interrupt
			SSP0IMSC = 4; //reciver 1/2 buffer fill int
		}
			break;
		case 1 :
		{
			VICVectAddr11 = isrAddr;
		    VICVectCntl11 = 0x20 | 11;     // use it for SSP1 Interrupt                
		    VICIntEnable |= 1 << 11;     //Enable SSP1 Interrupt  
			SSP1IMSC = 4; //reciver 1/2 buffer fill int			
		}
		break;
		default:
			break;
	}
#endif	
}

void  uController::SSPIni(unsigned char ifcNum, unsigned char dSz, unsigned char clkDiv, unsigned char fronts, unsigned long isrAddr)
{
	SSPPinCfg(ifcNum);
	(*(volatile unsigned long *)(SSP_BASE[ifcNum]  ))  = 0;
	SSPSetDataSize(ifcNum, dSz);
	SSPSetClkDiv(ifcNum, clkDiv);
	SSPSetfronts(ifcNum, fronts);
	if(isrAddr)
		SSPEnableInterrups(ifcNum, isrAddr);
	SSPEnable(ifcNum);       
}

unsigned short  uController::SSPExchange(unsigned char ifcNum, unsigned short data)
{

	(*(volatile unsigned long *)(SSP_BASE[ifcNum] + 8 )) = data;
//	(*(volatile unsigned long *)(SSP_BASE[ifcNum] + 8 )) = 0;
	while((*(volatile unsigned long *)(SSP_BASE[ifcNum] + 0xc )) == 0)
	{
	}
	return (*(volatile unsigned long *)(SSP_BASE[ifcNum] + 8 ));
}

unsigned short  uController::SSPFastExchange(unsigned char ifcNum, unsigned short data)
{
	while((*(volatile unsigned long *)(SSP_BASE[ifcNum] + 0xc )) == 0)
	{
	}
	(*(volatile unsigned long *)(SSP_BASE[ifcNum] + 8 ))  =  data;
	return (*(volatile unsigned long *)(SSP_BASE[ifcNum] + 8 ));
}


//
 void  uController::SSPWrite(unsigned char ifcNum, unsigned short data)
{
	(*(volatile unsigned long *)(SSP_BASE[ifcNum] + 8 )) = data;
	while((*(volatile unsigned long *)(SSP_BASE[ifcNum] + 0xc )) & 0x10)
	{
	}
	unsigned short tmp = (*(volatile unsigned long *)(SSP_BASE[ifcNum] + 8 ));
}


void  uController::SSPWriteOnly(unsigned char ifcNum, unsigned short data)
{
	(*(volatile unsigned long *)(SSP_BASE[ifcNum] + 8 )) = data;
	while((*(volatile unsigned long *)(SSP_BASE[ifcNum] + 0xc )) & 0x10)
	{
	}	
}


void  uController::SSPFastWrite(unsigned char ifcNum, unsigned short data)
{
	(*(volatile unsigned long *)(SSP_BASE[ifcNum] + 8 )) = data;
}
//


unsigned short uController::SSPRead(unsigned char ifcNum)
{
	while((*(volatile unsigned long *)(SSP_BASE[ifcNum] + 0xc )) & 0x10)
	{
	}
	return (*(volatile unsigned long *)(SSP_BASE[ifcNum] + 8 ));
}


 unsigned short  uController::SSPStartAndRead(unsigned char ifcNum)
{
	(*(volatile unsigned long *)(SSP_BASE[ifcNum] + 8 )) = 0;
	return SSPRead(ifcNum);
}



void uController::SSPWait(unsigned char ifcNum)
{
	while((*(volatile unsigned long *)(SSP_BASE[ifcNum] + 0xc )) == 0)
	{
	}
}

extern void uController::SSPSetDivs(unsigned char ifc, unsigned long div)
{
// 	unsigned short divPresc, divCR, divClk;
// 	if(div < 512)
// 	{
// 		divPresc = 2;
// 		divCR = div >> 2; 
// 		divClk =  1;
// 	}
// 	else if (div < 512 * 8)
// 	{
// 		divClk = 8;
// 		divPresc = 2;
// 		divClk =  (div >> 4) - 1;
// 	}
// 	else 
// 	{
// 		divClk = 8;
// 		divPresc = 254;
// 		divClk =  ((div >> 4)  /  127 ) - 1;
// 	}
// 	*(volatile unsigned long *)(SSP_BASE[ifc] + 0x10 ) = divPresc;
// 	*(volatile unsigned long *)(SSP_BASE[ifc] ) &= 0x00ff;
// 	*(volatile unsigned long *)(SSP_BASE[ifc] ) |= divPresc << 8;
	
}

void uController::SSPSetCtlRegDiv(unsigned char ifc, unsigned char div)
{
	unsigned short tmp  = div;
	*(volatile unsigned long *)(SSP_BASE[ifc] ) &= 0x00ff;
	*(volatile unsigned long *)(SSP_BASE[ifc] ) |= tmp << 8;
}
void uController::SSPSetPCLKDiv(unsigned char ifc, unsigned char div)
{
#ifdef LPC2478
	if(ifc == 0)
	{
		PCLKSEL1 &= ~(3 << 10);
		PCLKSEL1 |= GetPclkDiv(div) << 10;
	}	
	else if(ifc == 1)
	{
		PCLKSEL0 &= ~(3 << 20);
		PCLKSEL1 |= GetPclkDiv(div) << 10;
	}	
#endif
}

bool uController::SSPTxFifoIsEmpty(unsigned char ifc)
{	
	return (((*(volatile unsigned long *)(SSP_BASE[ifc] + 0xc )) & 1) == 0)? false : true; 
}


#ifdef LPC2478

/////////////////////////   == I2S
void uController::I2sIni(void)
{
//	p4.30 - fmt = 0 = p1.16 - md1	; p1.15 - md0 = 1;
	PCfg(4, 30, 0);
	PCfg(1, 15, 0);
	PCfg(1, 16, 0);
	IODIR1 |= 3 << 15;
	FIO4DIR |= 1 << 30;
	IOSET1 = 1 << 15;
	IOCLR1 = 1 <<  16;
	FIO4CLR = 1 << 30;

	PCONP |= 1 << 27;
	PCLKSEL0	&= ~(3 << 12);
	PCLKSEL0 |= 1 << 12;
	
	PCfg(0, 23, 2);
	PCfg(0, 24, 2);
	PCfg(0, 25, 2);

	I2S_DAI = 0x7e3;
uartSend	I2S_IRQ = 0x800;
}

void uController::I2sIni(unsigned long isrAddr, unsigned char level)
{	
		I2sIni();
		VICVectAddr31 = isrAddr;
		VICVectCntl31 = 0x20 | 31;     // use it for Timer 0 Interrupt
		VICIntEnable |= 0x80000000;  // Enable Timer0 Interrupt
	
		unsigned long irqR = level << 8;// | 1; 
		I2S_IRQ = irqR;
}

unsigned int uController::I2sRead (void)
{
	return I2S_RX_FIFO ;
}
void uController::I2sIntEnable(void)
{
	I2S_IRQ |= 1;
}
void uController::I2sIntDisable(void)
{
	I2S_IRQ &= ~1;
}

// /////////////////////////    I2S ==
#endif
// /////////////// fixxx

#ifdef LPC2478
void uController::PwmSEIni(unsigned int T, unsigned int duty)
{
	PCONP |= 1 << 6;
 	PCLKSEL0	&= ~(3 << 12);
 	PCLKSEL0 |= 1 << 12;
	PCfg(3, 27, 3);// 
 	
 
//  	VICVectAddr8 = (unsigned long) Pwm;           // set interrupt vector in 0
//   	VICVectCntl8 = 0x20 | 8;                     // use it for uart 0 Interrupt
//   	VICIntEnable |= 0x00000100;                   // Enable uart0 tx & rx Interrupt

//	PWMIR |= 0x70f;
	PWM1TCR = 2;//reset
	PWM1TCR = 8;//pwm enable
	PWM1MCR =  2;// reset on mr0
	PWM1PCR = 1 << 12; //wm4 enable//0x200;//wm1 output control enable
//
	PWM1MR0 = T;
	PWM1MR4 = duty;
//////////////////////////////////////////
	PWM1LER = 1 | (1 << 4);///3;	//mr0, mr1
	PWM1TCR = 9;// pwm & tc enable
}


void uController::PwmSESetDuty(unsigned int  duty)
{
 		PWM1MR1 = duty;
 		PWM1LER = 1 << 4;	// mr4
}

void PwmSESetT(unsigned int  t)
{
 		
	PWM1MR0 = t;
 	PWM1LER = 1;	//mr0
}
#endif


/////////////////////////===========uart===========
void uController::UartSetIsrAdd(unsigned char n, unsigned long isrAddr)
{
	switch(n)
	{
		case 0:
		{						
			VICVectAddr6  = isrAddr;
			VICVectCntl6 = 0x20 | 6;
			VICIntEnable |= 1 << 6;		
		}
		break;
		case 1:
		{						
			VICVectAddr7  = isrAddr;
			VICVectCntl7 = 0x20 | 7;
			VICIntEnable |= 1 << 7;		
		}
		break;
#ifdef LPC2478		
		case 2:
		{						
			VICVectAddr28  = isrAddr;
			VICVectCntl28 = 0x20 | 28;
			VICIntEnable |= 1 << 28;		
		}
		break;
		case 3:
		{						
			VICVectAddr29  = isrAddr;
			VICVectCntl29 = 0x20 | 29;
			VICIntEnable |= 1 << 29;		
		}
		break;
#endif		
		default:
			break;
	}
}

void uController::UartDisableInt(unsigned char n)
{
	switch(n)
	{
		case 0:
		{	
			U0IER  = 0;
			VICIntEnable &= ~(1 << 6);		
			VICVectCntl6 = 0;
			VICVectAddr6  = 0;
		}
		break;
		case 1:
		{						
			U1IER  = 0;
			VICIntEnable &= ~(1 << 7);		
			VICVectCntl7 = 0;
			VICVectAddr7  = 0;
		}
		break;
#ifdef LPC2478		
		case 2:
		{						
			U2IER  = 0;
			VICIntEnable &= ~(1 << 28);		
			VICVectCntl28 = 0;
			VICVectAddr28  = 0;
		}
		break;
		case 3:
		{	
			U3IER  = 0;
			VICIntEnable &= ~(1 << 29);						
			VICVectCntl29 = 0;
			VICVectAddr29  = 0;
					
		}
		break;
#endif		
		default:
			break;
	}
}

void uController::uartIni(unsigned char uartNum, bool rxEn, bool txEn, unsigned long isrAddr)
{
	switch(uartNum)
	{
#ifdef LPC2478		
		case 0:
		{			
			PCLKSEL0	&= ~(3 << 6);
			PCLKSEL0 |= 1 << 6;
			PINSEL0 &= ~(0xf << (2 * 2 )); //  
			PINSEL0 |=  (5 << (2 * 2)); // connect p0.2 & p0.3 to TxD0 and RxD0             
			U0LCR = 0x83;          // 8 bits, no Parity, 1 Stop bit 
			U0FDR = 0x7c;    		//fractal divider
			U0DLL = 12;            // 38400 Baud Rate @ 60MHz VPB Clock  
			U0LCR = 0x03;         // DLAB = 0 - Baud Rate fixed  

			if(rxEn)
				U0IER |= 1;
			else
				U0IER &= ~1;
			if(rxEn)
				U0IER |= 2;
			else
				U0IER &= ~2;
		}
			break;
		case 1:
		{
			PCLKSEL0	&= ~(3 << 8);
			PCLKSEL0 |= 1 << 8;
			PCfg(0, 15, 1);
			PCfg(0, 16, 1); // connect p0.15 & p0.16 to TxD1 and RxD1             
			U1LCR = 0x83;   //U0LCR = 0x83;          // 8 bits, no Parity, 1 Stop bit 
			U1FDR = 0x7c;    		//fractal divider
			U1DLL = 12;            // 38400 Baud Rate @ 60MHz VPB Clock  
			U1LCR = 0x03;         // DLAB = 0 - Baud Rate fixed 

			if(rxEn)
				U1IER |= 1;
			else
				U1IER &= ~1;
			if(rxEn)
				U1IER |= 2;
			else
				U1IER &= ~2;
		}		
			break;	
		case 2:
		{
			PCONP |= 1 << 24;
			PCLKSEL1	&= ~(3 << 16);
			PCLKSEL1 |= 1 << 16;
			PCfg(0, 10, 1);
			PCfg(0, 11, 1); // connect p0.10 & p0.11 to TxD2 and RxD2             
			U2LCR = 0x83;   //U0LCR = 0x83;          // 8 bits, no Parity, 1 Stop bit 
			U2FDR = 0x7c;    		//fractal divider
			U2DLL = 12;            // 38400 Baud Rate @ 60MHz VPB Clock  
			U2LCR = 0x03;         // DLAB = 0 - Baud Rate fixed  

			if(rxEn)
				U2IER |= 1;
			else
				U2IER &= ~1;
			if(rxEn)
				U2IER |= 2;
			else
				U2IER &= ~2;
		}		
			break;	
		case 3:
		{
			PCONP |= 1 << 25;
			PCLKSEL1 &= ~(3 << 18);
			PCLKSEL1 |= 1 << 18;
			PCfg(0, 0, 2);
			PCfg(0, 1, 2); // connect p0.15 & p0.16 to TxD3 and RxD3             
			U3LCR = 0x83;   //U0LCR = 0x83;          // 8 bits, no Parity, 1 Stop bit 
			U3FDR = 0x7c;    		//fractal divider
			U3DLL = 12;            // 38400 Baud Rate @ 60MHz VPB Clock  
			U3LCR = 0x03;         // DLAB = 0 - Baud Rate fixed  

			if(rxEn)
				U3IER |= 1;
			else
				U3IER &= ~1;
			if(rxEn)
				U3IER |= 2;
			else
				U3IER &= ~2;
		}		
			break;
#endif	
#ifdef LPC2148
		case 0:
		{			
			PCfg(0, 0, 1);
			PCfg(0, 1, 1);
			U0LCR = 0x83;          // 8 bits, no Parity, 1 Stop bit 
			U0FDR = 0x7c;    		//fractal divider
			U0DLL = 12;            // 38400 Baud Rate @ 60MHz VPB Clock  
			U0LCR = 0x03;         // DLAB = 0 - Baud Rate fixed  

			if(rxEn)
				U0IER |= 1;
			else
				U0IER &= ~1;
			if(rxEn)
				U0IER |= 2;
			else
				U0IER &= ~2;
		}
			break;
		case 1:
		{
			PCfg(0, 8, 1);
			PCfg(0, 9, 1); // connect p0.15 & p0.16 to TxD1 and RxD1             
			U1LCR = 0x83;   //U0LCR = 0x83;          // 8 bits, no Parity, 1 Stop bit 
			U1FDR = 0x7c;    		//fractal divider
			U1DLL = 12;            // 38400 Baud Rate @ 60MHz VPB Clock  
			U1LCR = 0x03;         // DLAB = 0 - Baud Rate fixed 

			if(rxEn)
				U1IER |= 1;
			else
				U1IER &= ~1;
			if(rxEn)
				U1IER |= 2;
			else
				U1IER &= ~2;
		}		
			break;					
#endif		
		default :  
			break;
	}	
	if(isrAddr != 0)
		UartSetIsrAdd(uartNum, isrAddr);
}

void uController::uartSend(unsigned char n, unsigned char b)
{
	switch (n)
	{
		case 0:
			U0THR = b;	
			break;
		case 1:
			U1THR = b;	
			break;
#ifdef LPC2478
		case 2:
			U2THR = b;	
			break;
		case 3:
		{
			U3THR = b;				
		}
			break;
#endif		
		default:
			break;
	}
}

char uController::UartGet(unsigned char n)
{
	char ret;
	switch (n)
	{
		case 0:
			ret = U0RBR;	
			break;
		case 1:
			ret = U1RBR;	
			break;
#ifdef LPC2478		
		case 2:
			ret = U2RBR;
			break;
		case 3:
		{			
			ret = U3RBR;				
		}			
			break;
#endif		
		default:
			ret = 0;
			break;
	}
	return ret;
}

unsigned short uController::UartGetIntSrc(unsigned char n)
{
	char ret;
	switch (n)
	{
		case 0:
			ret = U0IIR;	
			break;
		case 1:
			ret = U1IIR;	
			break;
#ifdef LPC2478		
		case 2:
			ret = U2IIR;
			break;
		case 3:
			ret = U3IIR;	
			break;
#endif		
		default:
			ret = 0;
			break;
	}
	return ret & 0xe;	
}


bool uController::UartTxIsReady(unsigned char n)
{
	char ret;
	switch (n)
	{
		case 0:
			ret = U0LSR & (1 << 5);	
			break;
		case 1:
			ret = U1LSR & (1 << 5);
			break;
#ifdef LPC2478		
		case 2:
			ret = U2LSR & (1 << 5);
			break;
		case 3:
			ret = U3LSR & (1 << 5);
			break;
#endif		
		default:
			ret = 0;
			break;
	}
	return (ret == 0)? false : true;	
}

// #else
// you need to define uC type;

// #endif

////                     LPC2478 ==
///////////// ==========RTC
void uController::RtcStart()
{
	RTC_CCR |= 1;
}

void uController::RtcStop()
{
	RTC_CCR &= ~1;
}


void SetBornDate()
{
	RTC_YEAR = 2013;
	RTC_MONTH = 2;
	RTC_DOM = 7;
	RTC_HOUR = 20; 
	RTC_MIN = 27;
	RTC_SEC = 00;
}

void  uController::RtcIni(void)
{
//	RTC_CCR = 2 | (1 << 4);//reset, clr src is prescaler
	RTC_CCR = 1 << 4;//reset, clr src is prescaler
//	RTC_CTIME0 = 0 | (0 << 8) | (0 << 16) | (0 << 24); 
	//RTC_CTIME1 = 1 | (1 << 8) | (2012 << 16); 
// 	RTC_SEC = 0;
// 	RTC_MIN = 0;
// 	RTC_HOUR = 0;
// 	RTC_DOM = 21;
// 	RTC_MONTH = 6;
// 	RTC_YEAR = 2012;
// 	int tmp = int (15000000/32768) - 1;
// 	RTC_PREINT = tmp;
// 	RTC_PREFRAC = 15000000 - ((tmp + 1) * 32768);
	RtcStart();
	
	unsigned char monthTable[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if(RTC_SEC > 59 || RTC_MIN > 59 || RTC_HOUR > 23 || RTC_DOM < 1 || RTC_MONTH < 1 || RTC_MONTH > 12 || RTC_YEAR < 2013 || RTC_YEAR > 2099)
	{
		SetBornDate();
		return;
	}
	if(RTC_DOM > monthTable[RTC_MONTH] || (RTC_MONTH == 2 && RTC_YEAR % 4 != 0 && RTC_DOM > 28))
		SetBornDate();
}

void uController::RtcSetSec(unsigned char val)
{
	if(val < 60)
	RTC_SEC = val;
}

unsigned char uController::RtcGetSec()
{	
	return RTC_SEC;
}

void uController::RtcSetMin(unsigned char val)
{
	if(val < 60)
	RTC_MIN = val;
}

unsigned char uController::RtcGetMin()
{	
	return RTC_MIN;
}

void uController::RtcSetHour(unsigned char val)
{
	if(val < 24)
	RTC_HOUR = val;
}

unsigned char uController::RtcGetHour()
{	
	return RTC_HOUR;
}

void uController::RtcSetDay(unsigned char val)
{
	if((val < 32) && (val > 0))
	RTC_DOM = val;
}

unsigned char uController::RtcGetDay()
{	
	return RTC_DOM;
}

void uController::RtcSetMonth(unsigned char val)
{
	if(val < 13)
	RTC_MONTH = val;
}

unsigned char uController::RtcGetMonth()
{	
	return RTC_MONTH;
}

void uController::RtcSetYear(unsigned short val)
{
	if(val < 4096)
	RTC_YEAR = val;
}

unsigned short uController::RtcGetYear()
{	
	return RTC_YEAR;
}


///////////// RTC==================
/////////////===============Timers
void  uController::IntervalGenerator(unsigned char nT, unsigned int nClk, unsigned long isrAddr)
{
	unsigned long baseAddr;
	switch(nT)
	{	
#ifdef LPC2478	
		case 0 :
			baseAddr = TMR0_BASE_ADDR;
			PCONP |= 1;	
			PCLKSEL0	&= ~(3 << 2);
			PCLKSEL0 |= 1 << 2;	
			break;
		case 1 : 
			baseAddr = TMR1_BASE_ADDR;
			PCONP |= 2;	
			PCLKSEL0	&= ~(3 << 4);
			PCLKSEL0 |= 1 << 4;
			break;
		case 2 : 
			baseAddr = TMR2_BASE_ADDR;
			PCONP |= 1 << 22;	
			break;
		case 3 :
			baseAddr = TMR3_BASE_ADDR;
			PCONP |= 1 << 23;	
			break;
#endif
#ifdef LPC2148
		case 0 :
			baseAddr = TMR0_BASE_ADDR;		
			break;
		case 1 : 
			baseAddr = TMR1_BASE_ADDR;	
			break;
#endif
		default :
		break;
		
	}
	(*(volatile unsigned long *)(baseAddr + 0x04)) = 2;//T1TCR = 2; // Stop and reset timer.
	(*(volatile unsigned long *)(baseAddr + 0x0C)) = 0;//T1PR  = 0x00; // Set prescaler to zero.
	(*(volatile unsigned long *)(baseAddr + 0x18)) = nClk;//T1MR0 = nClk; // Set Value.
	(*(volatile unsigned long *)baseAddr) = 0xff;//T1IR  = 0xff; // Reset all interrrupt flags.
	(*(volatile unsigned long *)(baseAddr + 0x14)) = 3;//T1MCR = 3; // Timer reset and Interrupt


	switch(nT)
	{	
		case 0 : 
		{
			VICVectAddr4 = isrAddr;
		    VICVectCntl4 = 0x20 | 4;     // use it for Timer 0 Interrupt
		    VICIntEnable |= 0x00000010;  // Enable Timer0 Interrupt
		}
			break;
		case 1 :
		{
			VICVectAddr5 = isrAddr;
		    VICVectCntl5 = 0x20 | 5;                     
		    VICIntEnable |= 0x00000020;                   
		}
			break;
#ifdef LPC2478		
		case 2 : 
		{
			VICVectAddr26 = isrAddr;
		    VICVectCntl26 = 0x20 | 26;                     
		    VICIntEnable |= 0x04000000;                   
		}
			break;
		case 3 : 
		{
			VICVectAddr27 = isrAddr;
		    VICVectCntl27 = 0x20 | 27;                     
		    VICIntEnable |= 0x08000000;                   
		}
			break;
#endif		
		default:
			break;
	}
	(*(volatile unsigned long *)(baseAddr + 0x04)) = 1;//T1TCR = 1;
}

void uController::TimerStop(unsigned char nT)
{	
	switch(nT)
	{	
		case 0 :	T0TCR  = 0;
			break;
		case 1 : 	T1TCR  = 0;
			break;
#ifdef LPC2478
		case 2 : 	T2TCR  = 0;
			break;
		case 3 : 	T3TCR = 0;
			break;
#endif		
		default:
			break;
	}
}

void uController::TimerStart(unsigned char nT)
{
	switch(nT)
	{	
		case 0 :	T0TCR  = 1;
			break;
		case 1 : 	T1TCR  = 1;
			break;
#ifdef LPC2478
		case 2 : 	T2TCR  = 1;
			break;
		case 3 : 	T3TCR = 1;
			break;
#endif		
		default:
			break;
	}
}
void uController::TimerReset(unsigned char nT)
{
	switch(nT)
	{	
		case 0 :	T0TCR  = 2;
			break;
		case 1 : 	T1TCR  = 2;
			break;
#ifdef LPC2478		
		case 2 : 	T2TCR  = 2;
			break;
		case 3 : 	T3TCR =  2;
			break;
#endif		
		default:
			break;
	}
}

/////////////Timers==================

///////////////////=============ADC=============================
void  uController::LPCadcIni(unsigned char num)
{
#ifdef LPC2148
	if(num == 0)
	{
		AD0CR &= ~(1 << 21); //PowerDown mode	
		AD0CR &= ~0xff00;
		AD0CR |= 14 << 8;// 4MHz work adcClock at 60 MHz input ADCClock 
		AD0CR &= ~0xf0000;
		AD0CR |= 1 << 16;// burst conversion 10bit
		AD0CR &= ~(7 << 26);//START == 000
		AD0CR |= 1 << 21; //PowerON	  
	}
	else 
	{
		AD1CR &= ~(1 << 21); //PowerDown mode	
		AD1CR &= ~0xff00;
		AD1CR |= 14 << 8;// 4MHz work adcClock at 60 MHz input ADCClock 
		AD1CR &= ~0xf0000;
		AD1CR |= 1 << 16;// burst conversion 10bit
		AD1CR &= ~(7 << 26);//START == 000
		AD1CR |= 1 << 21; //PowerON	 
	}
#endif
#ifdef LPC2478
	AD0CR &= ~(1 << 21); //PowerDown mode	
	PCONP |= 1 << 12;	//adcEnable
	AD0CR &= ~0xff00;
	AD0CR |= 14 << 8;// 4MHz work adcClock at 60 MHz input ADCClock 
	AD0CR &= ~0xf0000;
	AD0CR |= 1 << 16;// burst conversion 10bit
	AD0CR &= ~(7 << 26);//START == 000
	AD0CR |= 1 << 21; //PowerON				
#endif
}

void  uController::LPCadcChOn(unsigned char ch, unsigned char num)
{
#ifdef LPC2478	
	AD0CR |= 1 << ch;
#endif
#ifdef LPC2148
	if(num == 0)
		AD0CR |= 1 << ch;
	else
		AD1CR |= 1 << ch;
#endif
}

void  uController::LPCadcChOff(unsigned char ch, unsigned char num)
{
#ifdef LPC2478	
	AD0CR &= ~(1 << ch);
#endif
#ifdef LPC2148
	if(num == 0)
		AD0CR &= ~(1 << ch);
	else
		AD1CR &= ~(1 << ch);
#endif
}

unsigned short  uController::LPCadcRead(unsigned char ch, unsigned char num)
{
	volatile unsigned long* adDataReg;
#ifdef LPC2478	
	adDataReg =  (volatile unsigned long *)(AD0_BASE_ADDR + 0x10);
#endif
#ifdef LPC2248
	if(num == 0)
		adDataReg =  (volatile unsigned long *)(AD0_BASE_ADDR + 0x10);
	else
		adDataReg =  (volatile unsigned long *)(AD1_BASE_ADDR + 0x10);
#endif
	adDataReg += ch;
	while(  (*adDataReg & 0x80000000) == 0)
	{	
	}
	return (*adDataReg >> 6) & 0x3ff;
}

///////////////=================DAC==========



////////   == DAC
void uController::DacIni(void)
{
	#ifdef LPC2148
	PINSEL1 &= ~0x000c0000;
    PINSEL1 |=  0x00080000; // p0.25 configurated as dac
	#endif
	#ifdef LPC2478
	PCfg(0, 26, 2);
	#endif	
}



void uController::DacOut(unsigned short data)
{
	DACR = data << 6;
}
////////////////   DAC ==
///////////////////////////=======2013============///////////////////////////
unsigned long uController::GetPclkDiv(unsigned int div)
{
	unsigned char d;
	switch (div)
	{
		case 1: d = 1;
			break;
		case 2: d = 2;
			break;
		case 4: d = 0;
			break;
		case 8: d = 3;
			break;
		default : d = 0;
	}	
	return d;
}

//////////===============GPIO===============
Gpio::Gpio(unsigned char portGroup, unsigned char portNum, bool isOut, unsigned long isr)
{
	group = portGroup;
	num = portNum;
	PCfg(group, num, 0);
	SetDir(isOut);

}
 
void Gpio::Toggle(void) 
{
 	PTg(group, num);
}

void Gpio::High(void) 
{
	PHigh(group, num);
}


void Gpio::Low(void) 
{
	PLow(group, num);
}

bool Gpio::GetState(void)
{
 	return 	 PGet(group, num);
}
void Gpio::SetState(bool state)
{
 	if(state)
		PHigh(group, num);
	else
		PLow(group, num);
}

void Gpio::SetDir(bool dir) 
{
	   PDir(group, num, dir)  ;
}
#ifdef LPC2478
void Gpio::SetActivR(void) 
{
	if(group == 0)
	{
		IO0_INT_EN_R |= 1 << num;
	}
	else if(group == 2)
	{
		IO2_INT_EN_R |= 1 << num;
	}
}


void Gpio::SetActivF(void) 
{
	if(group == 0)
	{
		IO0_INT_EN_F |= 1 << num;
	}
	else if(group == 2)
	{
		IO2_INT_EN_F |= 1 << num;
	}
}

void Gpio::SetActivFR(void) 
{
	if(group == 0)
	{
		IO0_INT_EN_R |= 1 << num;
		IO0_INT_EN_F |= 1 << num;
	}
	else if(group == 2)
	{
		IO2_INT_EN_R |= 1 << num;
		IO2_INT_EN_F |= 1 << num;
	}
}


void Gpio::SetPassivR(void) 
{
	if(group == 0)
	{
		IO0_INT_EN_R &= ~(1 << num);
	}
	else if(group == 2)
	{
		IO2_INT_EN_R &= ~(1 << num);
	}
}


void Gpio::SetPassivF(void) 
{
	if(group == 0)
	{
		IO0_INT_EN_F &= ~(1 << num);
	}
	else if(group == 2)
	{
		IO2_INT_EN_F &= ~(1 << num);
	}
}

void Gpio::SetPassivFR(void) 
{
	if(group == 0)
	{
		IO0_INT_EN_R &= ~(1 << num);
		IO0_INT_EN_F &= ~(1 << num);
	}
	else if(group == 2)
	{
		IO2_INT_EN_R &= (1 << num);
		IO2_INT_EN_F &= (1 << num);
	}
}



void Gpio::SetIntAddr(unsigned long addr) 
{
	if(addr)
	{
		VICVectAddr17  = addr;
		VICVectCntl17 = 0x20 | 17;
		VICIntEnable |= 1 << 17;
	}
}
#endif
//interrupt must be redefined for other proc


Gpio::~Gpio(void) 
{

}
///////////////////////============GPIO end ==============///////////////////

//////////////////////============= CAP  ==============///////////////////////
Cap::Cap(unsigned char nTim, unsigned char nCap, unsigned char pG, unsigned char pN,
bool isFCap, bool isRCap, unsigned char pclkDiv, unsigned int presc, unsigned long isr, bool intEn)
{
	capNum = nCap;
	timNum = nTim;	
	switch(timNum)
	{		
		case 0:	
		{
			ctlReg = &T0TCR; 
			capCtlReg = &T0CCR;
			tcCtlReg = &T0CTCR;
			evFlagReg = &T0IR;			
			prescReg = &T0PR;
#ifdef LPC2478			
			capReg =  (capNum == 0) ?  &T0CR0 : &T0CR1;
#endif
#ifdef LPC2148
			if(capNum == 0)
				capReg = &T0CR0;
			else if(capNum == 1)
				capReg = &T0CR1;
			else if(capNum == 2)
				capReg = &T0CR2;
			else if(capNum == 3)
				capReg = &T0CR3;

#endif			
			vicVectAddrReg = &VICVectAddr4;
			vicVectCntReg = &VICVectCntl4;
			intNum  = 4;	
		}
			break;
		case 1:	
		{
			ctlReg = &T1TCR; 
			capCtlReg = &T1CCR;
			tcCtlReg = &T1CTCR;
			evFlagReg = &T1IR;			
			prescReg = &T1PR;
#ifdef LPC2478				
			capReg =  (capNum == 0) ?  &T1CR0 : &T1CR1;
#endif		
#ifdef LPC2148
			if(capNum == 0)
				capReg = &T1CR0;
			else if(capNum == 1)
				capReg = &T1CR1;
			else if(capNum == 2)
				capReg = &T1CR2;
			else if(capNum == 3)
				capReg = &T1CR3;

#endif					
			vicVectAddrReg = &VICVectAddr5;
			vicVectCntReg = &VICVectCntl5;
			intNum  = 5;	
		}
			break;
#ifdef LPC2478		
		case 2:	
		{
			ctlReg = &T2TCR; 
			capCtlReg = &T2CCR;
			tcCtlReg = &T2CTCR;
			evFlagReg = &T2IR;			
			prescReg = &T2PR;
			capReg =  (capNum == 0) ?  &T2CR0 : &T2CR1;
			vicVectAddrReg = &VICVectAddr26;
			vicVectCntReg = &VICVectCntl26;
			intNum  = 26;	
		}
			break;
		case 3:	
		{
			ctlReg = &T3TCR; 
			capCtlReg = &T3CCR;
			tcCtlReg = &T3CTCR;
			evFlagReg = &T3IR;			
			prescReg = &T3PR;
			capReg =  (capNum == 0) ?  &T3CR0 : &T3CR1;
			vicVectAddrReg = &VICVectAddr27;
			vicVectCntReg = &VICVectCntl27;
			intNum  = 27;	
		}
			break;
#endif		
		default: 
			break;
	}
	if((timNum < 4) && (capNum < 4))
	{
		#ifdef LPC2478
		if(pG < 2)
			PCfg(pG, pN, 3);
		else if (pG < 4)
			PCfg(pG, pN, 2);
		#endif
		#ifdef LPC2148
		if((pN < 16) ||(pN == 22) || (pN == 28) || (pN == 29))
			PCfg(0, pN, 2);
		else if ((pN == 16) || (pN == 19) || (pN == 21) || (pN == 30))
			PCfg(pG, pN, 3);
		else if ((pN == 17) || (pN == 18))
			PCfg(pG, pN, 1);
		#endif
		
		
		tcCtlReg = 0;
		SetPclkDiv(pclkDiv);
		SetPrescaler(presc);
		DisableCap();
		if(isFCap)
			EnableFCap();
		if(isRCap)
			EnableRCap();		
		*ctlReg = 2;		//	reset
		*ctlReg = 1;		//	enable
		SetIsrAddr(isr);
		if(intEn)
			EnableInt();
		else
			DisableInt();
	}	
}

void Cap::DisableCap(void)
{ 
	*capCtlReg &= ~(3 << (capNum * 3));
}

void Cap::EnableFCap(void)//capture on falling edge
{ 
	*capCtlReg |= 2 << (capNum * 3);
}

void Cap::EnableRCap(void) //capture on rising edge
{ 
	*capCtlReg |= 1 << (capNum * 3);
}

void Cap::EnableOnlyFCap(void)//capture only on falling edge
{ 
	*capCtlReg &= ~(1 << (capNum * 3));
	*capCtlReg |= 2 << (capNum * 3);
}

void Cap::EnableOnlyRCap(void) //capture only on rising edge
{ 
	*capCtlReg &= ~(2 << (capNum * 3));
	*capCtlReg |= 1 << (capNum * 3);
}

void Cap::EnableFRCap(void) //capture on rising & falling edge
{
	*capCtlReg |= 3 << (capNum * 3);
}

void Cap::EnableInt(void)
{
	*capCtlReg |= 4 << (capNum * 3);
}

void Cap::DisableInt(void)
{
	*capCtlReg &= ~(4 << (capNum * 3));
}

void Cap::SetIsrAddr(unsigned long addr)
{
	if(addr)
	{
		*vicVectAddrReg  = addr;
		*vicVectCntReg = 0x20 | intNum;
		VICIntEnable |= 1 << intNum;
	}
}


void Cap::SetPclkDiv(unsigned int div)
{
	#ifdef LPC2478
	switch(timNum)
	{		
		case 0:	
		{
			PCLKSEL0 |=  GetPclkDiv(div) << 2;
			PCONP |= 2;
		}
		break;
		case 1:	
		{
			PCLKSEL0 |=  GetPclkDiv(div) << 4;
			PCONP |= 4;
		}
		break;
		case 2:	
		{
			PCLKSEL1 |=  GetPclkDiv(div) << 12;
			PCONP |= 1 << 22;
		}
		break;
		case 3:	
		{
			PCLKSEL1 |=  GetPclkDiv(div) << 14;
			PCONP |=  1 << 23;
		}
		break;
	}	
	#endif
}

void Cap::SetPrescaler(unsigned int pr)
{
	*prescReg = pr;
}

unsigned int Cap::GetT(void)
{
	return *capReg;
}

bool Cap::IsCap(void)
{
	return (*evFlagReg & (1 << (4 + capNum))) ? true : false;
}

void Cap::ResetFlags(void)
{
	*evFlagReg	= 0xff;
}

Cap::~Cap()
{

}
// //////////////////////============= CAP end ==============///////////////////////
// //////////////////////============= SSP 
RegsSPI*  spi0 = (RegsSPI *) SSP0_BASE_ADDR;
#ifdef LPC2478
RegsSSP* ssp0 = (RegsSSP *) SSP0_BASE_ADDR;
#endif
RegsSSP* ssp1 = (RegsSSP *) SSP1_BASE_ADDR;


Spi::Spi(unsigned long ifcStructAddr)	
{
	pIfc = ifcStructAddr;
}

Spi::~Spi()
{

}


void Spi::SetDataSize(unsigned char sz)
{

	if(sz == 8)
		((RegsSPI*) pIfc)->spcr &= ~2;
	else if(sz > 8)
	{
		((RegsSPI*) pIfc)->spcr |= 2;
		((RegsSPI*) pIfc)->spcr |= (sz & 0xf) << 8;
	}	
}

void Spi::Setfronts(unsigned char f)
{
	((RegsSPI*) pIfc)->spcr &= 0x18;
	((RegsSPI*) pIfc)->spcr |= f << 3;
}

void Spi::Enable()
{

}

void Spi::Disable()
{

}


void Spi::FastWr(unsigned short data)
{	
	((RegsSPI*) pIfc)->spdr = data;
}


unsigned short Spi::FastRd()
{
	return ((RegsSPI*) pIfc)->spdr;
}


unsigned short Spi::FastRW(unsigned short data)
{
	unsigned short ret = ((RegsSPI*) pIfc)->spdr;
	((RegsSPI*) pIfc)->spdr = data;
	return ret;
}


void Spi::Wait()
{
	while(( ((RegsSPI*) pIfc)->spsr & 0X80) == 0)
	{
	}
}


void Spi::Write(unsigned short data)
{
	((RegsSPI*) pIfc)->spdr = data;
	while((((RegsSPI*) pIfc)->spsr & 0X80) == 0)
	{
	}	
	unsigned short tmp = ((RegsSPI*) pIfc)->spdr;
}


unsigned short Spi::Read()
{
	((RegsSPI*) pIfc)->spdr = 0;
	while((((RegsSPI*) pIfc)->spsr & 0X80) == 0)
	{
	}	
	return ((RegsSPI*) pIfc)->spdr;
}



unsigned short Spi::RW(unsigned short data)
{
	((RegsSPI*) pIfc)->spdr = data;
	while((((RegsSPI*) pIfc)->spsr & 0X80) == 0)
	{
	}	
	return ((RegsSPI*) pIfc)->spdr;
}

unsigned long Spi::GetStatus()
{
	return ((RegsSPI*) pIfc)->spsr;
}

bool Spi::TxIsEmpty()
{
	return ((((RegsSPI*) pIfc)->spsr & 0X80) == 0)? true : false ;
}





void Spi::EnableInterrups(unsigned long isrAddr, unsigned char intType)
{
	VICVectAddr10 = isrAddr;
	VICVectCntl10 = 0x20 | 10;     // use it for SSP0 Interrupt
	VICIntEnable |= 1 << 10;      //Enable SSP0 Interrupt
	((RegsSPI*) pIfc)->spcr |= 1 << 7;
}

void Spi::DisableInterrups()
{
	((RegsSPI*) pIfc)->spcr &= ~(1 << 7);
}

void Spi::SetDiv(unsigned int div)
{
	if(div > 244)
		div = 244;
	else if (div < 8)
		div = 8;
	div &= ~1;
	((RegsSPI*) pIfc)->spccr = (unsigned char) div;
}

void Spi::Start()
{
	if(TxIsEmpty())
		((RegsSPI*) pIfc)->spdr = 0;
}



Ssp::Ssp(unsigned long ifcStructAddr)	
{
	pIfc = ifcStructAddr;
}
Ssp::~Ssp()
{

}

void Ssp::SetDataSize(unsigned char sz)
{
	( (RegsSSP*) pIfc )->cr0  &= ~0xf;
 	( (RegsSSP*) pIfc )->cr0 |= (sz - 1);
 }

void Ssp::Setfronts(unsigned char f)
{	
	( (RegsSSP*) pIfc )->cr0 &= ~(3 << 6) ;
	( (RegsSSP*) pIfc )->cr0 |= 	(f << 6) ;
}

void Ssp::Enable()
{
	( (RegsSSP*) pIfc )->cr1 |= 2;
}

void Ssp::Disable()
{
	( (RegsSSP*) pIfc )->cr1 &= ~2;//enable spi;
}

void Ssp::FastWr(unsigned short data)
{	
//	unsigned short ret = ( (RegsSSP*) pIfc )->dr;
	( (RegsSSP*) pIfc )->dr = data;
}

unsigned short Ssp::FastRd()
{
	return ( (RegsSSP*) pIfc )->dr;
}

unsigned short Ssp::FastRW(unsigned short data)
{
	unsigned short ret = ( (RegsSSP*) pIfc )->dr;
	( (RegsSSP*) pIfc )->dr = data;
	return ret;
}

void Ssp::Wait()
{
	while((( (RegsSSP*) pIfc )->sr & 0x10) != 0)
	{
	}
}

void Ssp::Write(unsigned short data)
{
	( (RegsSSP*) pIfc )->dr = data;
 	while((( (RegsSSP*) pIfc )->sr & 0x10) != 0)
 	{
 	}	
	unsigned short tmp = ( (RegsSSP*) pIfc )->dr;
}

unsigned short Ssp::Read()
{
	( (RegsSSP*) pIfc )->dr = 0;
	while((( (RegsSSP*) pIfc )->sr & 1) == 0)
	{
	}	
	return ( (RegsSSP*) pIfc )->dr;
}

unsigned short Ssp::RW(unsigned short data)
{
	( (RegsSSP*) pIfc )->dr = data;
	while((( (RegsSSP*) pIfc )->sr & 0x10) != 0)
	{
	}	
	return ( (RegsSSP*) pIfc )->dr;
} 

unsigned long Ssp::GetStatus()
{
	return ( (RegsSSP*) pIfc )->sr;
}

bool Ssp::TxIsEmpty()
{
	return ((( (RegsSSP*) pIfc )->sr & 1) == 0)? false : true;
}


void Ssp::EnableInterrups(unsigned long isrAddr, unsigned char intType)
{
	VICVectAddr11 = isrAddr;
	VICVectCntl11 = 0x20 | 11;     // use it for SSP1 Interrupt                
	VICIntEnable |= 1 << 11;     //Enable SSP1 Interrupt  
	( (RegsSSP*) pIfc )->imsc = intType; //reciver 1/2 buffer fill int
}

void Ssp::DisableInterrups()
{
	( (RegsSSP*) pIfc )->imsc = 0;
}

void Ssp::SetDiv(unsigned int div)
{	
	div &= ~1;
	if(div > (254 * 255))
		div = 254 * 255;
	else if (div < 2)
		div = 2;
	if (div > 254)
	{
		unsigned int tmp = div / 254;
		( (RegsSSP*) pIfc )->cr0 |= (tmp - 1) << 8; 
		( (RegsSSP*) pIfc )->cpsr = 254;
	}
	else
	{
		( (RegsSSP*) pIfc )->cpsr = (unsigned char) div;
		( (RegsSSP*) pIfc )->cr0 &= 0x00ff; 
	}
}

void Ssp::Start()
{
	if(TxIsEmpty())
		for(int i = 0; i < 4; i++)
			( (RegsSSP*) pIfc )->dr = 0;
	
}


