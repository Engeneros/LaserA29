#ifndef ABSTRACT_SERIAL_IO_H
#define ABSTRACT_SERIAL_IO_H

namespace Board
{
	class AbstractSerialIO
	{
	public:
		virtual unsigned int GetState() const = 0;
		virtual void SetState(unsigned int state, unsigned int bMask = 0xffffffff) = 0;
	};
	
	class AbstractIoSSP 
	{
	public: 
		virtual void SspAdj () = 0;
		virtual void Set(unsigned short val) = 0;
		virtual unsigned short Get() = 0;
		virtual void BuffWr(unsigned short val) = 0;
		virtual void SetState(unsigned short state, unsigned short bMask = 0xffff) = 0;
		virtual unsigned short BuffRd() = 0;
		virtual void Out() = 0;
		virtual void In() = 0;
		virtual void Select() = 0;
		virtual void Release() = 0;
		virtual AbstractIoSSP* Next() = 0;
		virtual void SetNext(AbstractIoSSP* nx) = 0;
		virtual AbstractIoSSP* Prev() = 0;
		virtual void SetPrev(AbstractIoSSP* pr) = 0;
	};

}
#endif
