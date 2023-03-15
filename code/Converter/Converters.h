#ifndef CONVERTERS_H
#define CONVERTERS_H
#include <AbstractConverter.h>

namespace Board
{		
		class Converter : public AbstractConverter
		{	
		public:
			Converter (double xo, double x, double y);
			virtual ~Converter();				
				virtual double Convert(double x);
				virtual void AddPoint(double x, double y);
				virtual ClbrPoint* GetPoint(double x);
				virtual ClbrPoint* GetFirstPoint()  const;
				virtual ClbrPoint* GetLastPoint()  const ;
				virtual ClbrPoint* Next (ClbrPoint*)  const;
				virtual ClbrPoint* Prev(ClbrPoint*)  const; 
				virtual ClbrPoint* Curr() const;
				virtual void SetCurr(ClbrPoint* p);
				virtual double Reverse(double y);
				virtual void DeletePoint(ClbrPoint* p);
				virtual void DeleteAll();
				virtual unsigned int GetNumPoints();
		protected:
			void Erase();
			ClbrPoint* FindNextDoor(double x);
			ClbrPoint* FindNextDoorY(double y);
			ClbrPoint* clbrList;
			ClbrPoint* fin;
			ClbrPoint* curr;
		};
}
#endif

