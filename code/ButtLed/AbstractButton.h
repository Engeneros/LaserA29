#ifndef ABSTRACT_BUTTON_H
#define ABSTRACT_BUTTON_H

namespace Board
{
	class AbstractButton
	{
	public:
		virtual bool IsPressed() const = 0;
	};
}
#endif
