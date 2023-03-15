#ifndef EXCHANGE_BUFFERS_H
#define EXCHANGE_BUFFERS_H
namespace Board
{ 
	static const unsigned int MAX_RX_FIFO_SIZE = 64;
	static const unsigned int MAX_RX_BUFFS = 4;
	static const char END_RX = 0;
	static const char END_TX = 0;
	static const char END_RX1 = 10;
	static const char END_RX2 = 13;
	static const char BG_CMD = '#';
	static const char SEPARATOR = ' ';
	static const char BG_PRM = '=';
	static const char NEXT_PRM = ',';
	static const char END_CMD = ';';
	
	class Message
	{
	public:
		Message(unsigned int sz = 0);
		unsigned int GetSize() const;
		void SetSize(unsigned int s);
		void GetData(char* d);
		char* GetBuf();
		void SetData(const char* d);
		void SetNext(Message* m);		
		Message* GetNext();
		~Message();
	private:
		unsigned int size;
		char* buf;
		Message* nxt; 
	};

///////////////////////////////////////	
	struct RxBuffInfo 
	{
		unsigned int bgAddr;
		unsigned int sz;
		bool ready;
		RxBuffInfo* next;
	};
	
	class TextRxCircle
	{
	public:
		TextRxCircle(unsigned int sz = MAX_RX_FIFO_SIZE, unsigned int nBuffs = MAX_RX_BUFFS);
		unsigned int GetBufSz() const;
		void AddSym(char symb);
		void GetMessage(char* uBuff);
		bool IsReady();	
		~TextRxCircle();
	private:	
		void Finish();
		char* fifo;
		int size, nB;
		unsigned int cAddr, bAddr;// adreess in fifo
		RxBuffInfo* hwWr;
		RxBuffInfo*	usrRd;
	};

//////////////////////////////////////
	class TextMessageTx
	{
	public:
		TextMessageTx();
		char SendSym();
		void CopyMessage(const char* wr);
		TextMessageTx* GetNext();
		void SetNext(TextMessageTx* nM);
		bool IsReady();
		~TextMessageTx();
	private:	
		bool isReady;
		char *buf;
		unsigned int bufSz;
		unsigned int ptr;
		TextMessageTx* next;	
	};
	
///////////////////////////////////////

	bool GetCmd(const char* in, char* cmd, int* prm, int len);
	
}

#endif
