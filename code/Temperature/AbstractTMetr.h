#ifndef ABSTRACT_T_METR_H
#define ABSTRACT_T_METR_H

namespace Board
{
	class AbstractTmetr
	{
	public:
		virtual void IntEvent() = 0;
		virtual double GetT () = 0;
	};
}

#endif

