#ifndef ABSTRACT_ENCODER_H
#define ABSTRACT_ENCODER_H

namespace Board
{
	class AbstractEncoder
	{
	public:
		virtual int GetAccumulatedValue() = 0;
	};
}
#endif
