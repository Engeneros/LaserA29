#ifndef ABSTRACT_CONVERTER_H
#define ABSTRACT_CONVERTER_H

namespace Board
{	
			struct ClbrPoint
		{
			ClbrPoint();
			ClbrPoint(double ar, double f, ClbrPoint* nx = 0, ClbrPoint* pr = 0);
			double x;
			double y;
			ClbrPoint* next;
			ClbrPoint* prev;
		};
		
		class AbstractConverter
		{
		public:
				virtual ~AbstractConverter() {}
				virtual double Convert(double x) = 0;
				virtual void AddPoint(double x, double y)  = 0;
				virtual ClbrPoint* GetPoint(double x) = 0;
				virtual ClbrPoint* GetFirstPoint()  const = 0;
				virtual ClbrPoint* GetLastPoint()  const = 0;
				virtual ClbrPoint* Next (ClbrPoint* p)  const = 0;
				virtual ClbrPoint* Prev(ClbrPoint* p)  const = 0;
				virtual ClbrPoint* Curr() const = 0;
				virtual void SetCurr(ClbrPoint* p) = 0;
				virtual double Reverse(double y)  = 0;
				virtual void DeletePoint(ClbrPoint* p) = 0;
				virtual void DeleteAll() = 0;
				virtual unsigned int GetNumPoints() = 0;
		};
}
#endif


