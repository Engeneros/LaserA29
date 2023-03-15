#include <iap.h>
#include <LPC214x.H>
#include <Delay.h>
using namespace HwSleep;

int dcSumm(int A, int B){
	return A + B;
}

unsigned long theSaver::sectNumber(unsigned long addr){
	unsigned long ret;
	if(addr < 0x8000 )
		ret = addr >> 12;
	else if(addr < 0x00078000 )
		ret = (((addr - 0x8000)) >> 15) + 8;
	else 
		ret = (((addr - 0x00078000)) >> 12) + 22;

	return ret;
}

char theSaver::prepareSectors(unsigned long sNbeg, unsigned long sNend){
	  VICIntEnClr = 0xffffffff;//interrupts Disable
	  command[0] = PREPARE_SECTRS;
	  command[1] = sNbeg;
	  command[2] = sNend;

	  iap_entry(command, result);
      delay_ms(500);
	  return result[0];
}
char theSaver::eraseSectors(unsigned long sNbeg, unsigned long sNend){
	 VICIntEnClr = 0xffffffff;//interrupts Disable
	 command[0] = ERASE_SECTORS;
	 command[1] = sNbeg;
	 command[2] = sNend;
	 command[3] = sysFreq_KHz;

	 iap_entry(command, result);
	 delay_ms(500); 

	 return result[0];
}
char theSaver::writeData(unsigned long flashAddr, char* buf,  unsigned short nBytes){
     VICIntEnClr = 0xffffffff;//interrupts Disable
     command[0] = COPY_RAM_2_FLASH;
	 command[1] = flashAddr;//(((unsigned long) &vTT[0]) + 256) & 0xffffff00;
	 command[2] = (unsigned int)buf;//(unsigned long) &flashData[0];//(((unsigned long) &flashData)+ 16) & 0xffffffe; 
	 command[3] = nBytes;
	 command[4] = sysFreq_KHz;
		
	 iap_entry(command, result);
	 delay_ms(500);
	 return result[0];
}


theSaver::theSaver (unsigned long freq_KHz){
	iap_entry = (IAP) IAP_LOCATION;
	sysFreq_KHz = freq_KHz;
}

theSaver::~theSaver (){
//to fix: restore interrupts
}

char theSaver::save (unsigned long addr, unsigned short sz){
	char ret = 0;
	unsigned long sn = sectNumber(FLASH_DATA_ADDR);
	ret = prepareSectors(sn, sn);
	if(ret == 0)
		ret = eraseSectors(sn, sn);
		if(ret == 0)
			ret = prepareSectors(sn, sn);
			if(ret == 0)
				ret = writeData(FLASH_DATA_ADDR, (char*) addr, 256);		
	return ret;
}

char theSaver::remember (unsigned long addr, char* buf, unsigned short sz){
	unsigned short i;
	const unsigned char*  flashPointer  = (const unsigned char*) FLASH_DATA_ADDR;

	for(i = 0; i < sz; i++)
		*buf++ =  *flashPointer++;
	return 0;
}
