#ifndef ABSTRACT_UART_H
#define ABSTRACT_UART_H
namespace Board
{
	 
	class AbstractUART
	{
	public:	  
		virtual unsigned int GetMessageSize() const = 0;
		virtual void GetMessage(char* buf) = 0;
		virtual void SendMessage(const char* buf) = 0;
		virtual void SendSymbol( char sym) = 0;
		virtual void Update() = 0;
	};
}
#endif
