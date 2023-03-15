#ifndef ABSTRACT_LED_H
#define ABSTRACT_LED_H

namespace Board
{
	class AbstractLED
	{
	public:
		virtual bool IsOn() const = 0;
		virtual void SetOn(bool on) = 0;
		virtual void Toggle() = 0;
	};
}
#endif
