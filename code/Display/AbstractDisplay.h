#ifndef ABSTRACT_DISPLAY_H
#define ABSTRACT_DISPLAY_H

namespace Board
{
	 static const unsigned char MIDDLE_FONT  = 0;
	 static const unsigned char ARIALCYR28 = 1;
	 static const unsigned char VOID_FONT  = 255;
	 static const unsigned char N_FONTS = 2;
	
	class AbstractDisplay
	{
	  public:
		virtual int WriteStr(int x, int  y, const char* str, const int font, int pitch, bool isInverted) = 0;
		virtual void Erase() = 0;
	};
}
#endif




