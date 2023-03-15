#ifndef DC_IAP
#define DC_IAP

extern int dcSumm(int A, int B);

class dataSaver {
	private:
	public:
		
	virtual char save (unsigned long addr, unsigned short sz) = 0; // чистая виртуальная функция
	virtual char remember (unsigned long addr, char* buf, unsigned short sz) = 0; // чистая виртуальная функция
} ;

class theSaver : public dataSaver{
	private:
	
	static const unsigned long PREPARE_SECTRS = 50;//10;
 	static const unsigned long COPY_RAM_2_FLASH = 51;//10;
	static const unsigned long ERASE_SECTORS = 52;

	static const unsigned long CMD_SUCCESS = 0;// Command is executed successfully.
	static const unsigned long INVALID_COMMAND = 1;// Invalid command.
	static const unsigned long SRC_ADDR_ERROR  = 2;//Source address is not on a word boundary.
	static const unsigned long DST_ADDR_ERROR = 3;//Destination address is not on a correct boundary.
	static const unsigned long SRC_ADDR_NOT_MAPPED = 4;//Source address is not mapped in the memory map.

	static const unsigned long IAP_LOCATION = 0x7ffffff1 ;
	unsigned int command[5];
	unsigned int result[2];
	unsigned long sysFreq_KHz;

	typedef void (*IAP)(unsigned int [],unsigned int[]);
	IAP iap_entry;

	
	static const unsigned long FLASH_DATA_ADDR = 0x0007C000;	 //the last sector begin
	

	unsigned long sectNumber(unsigned long addr);
	char prepareSectors(unsigned long sNbeg, unsigned long sNend);
	char eraseSectors(unsigned long sNbeg, unsigned long sNend);
	char writeData(unsigned long flashAddr, char* buf,  unsigned short nBytes);


	public:
	theSaver ( unsigned long freq_KHz );
	~theSaver ();
	virtual char save (unsigned long addr, unsigned short sz); // чистая виртуальная функция
	virtual char remember (unsigned long addr, char* buf, unsigned short sz); // чистая виртуальная функция

};

#endif
