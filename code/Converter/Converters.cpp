#include <Converters.h>

#include <math.h>

using namespace Board;	
	ClbrPoint::ClbrPoint()
	{
		next = prev = 0;
	}
	
	ClbrPoint::ClbrPoint(double ar, double f, ClbrPoint* nx, ClbrPoint* pr)
	{
		x = ar;
		y = f;
		next = nx;
		prev = pr;
	}
	
	Converter::Converter (double xo, double x, double y)
	{
		clbrList = fin = 0;
	}
	
	Converter::~Converter()
	{
		Erase();
	}
	
	double Converter::Convert(double x) 
	{
		double ret;
		ClbrPoint * rgh = FindNextDoor(x);
		if(rgh != 0)                             //  point x not more rights than fin->x
		{
			if(rgh->x != x)                    //  point x  lefts that fin->x
			{
				if(rgh->prev != 0)          //  point x  is inside pointsList
				{
					double x0 = rgh->prev->x , x1 = rgh->x;
					double y0 = rgh->prev->y,  y1 = rgh->y;
					double dx = x1 - x0, dy = y1 - y0;
					if((double) abs(dx) > 0.005)
						ret = y0 + (x - x0) * dy/dx;
					else
						ret = y0;
				}
				else                   //    point x is not inside pointslist. It is lefts
						ret = rgh->y;					
			} 
			else                       //    x is one points of pointslist
				ret = rgh->y;
		}
		else if(fin != 0)           //     point x is outside of pointlist (more right) but poitslist is not empty
		{
			if(fin->prev != 0)    //     and have 2 or more points
			{
				double x0 = fin->prev->x , x1 = fin->x;
				double y0 = fin->prev->y,  y1 = fin->y;
				double dx = x1 - x0, dy = y1 - y0;
				if((double) abs(dx) > 0.005)
					ret = y1 + (x - x1) * dy/dx;
				else
					ret = y1;
			}
			else                   //         pointsList have 1 point == fin
				ret = fin->y;
		}
		else	                      //         pointslist is empty 		
			ret = 0.0;
		return ret;
	}
	
	double Converter::Reverse(double y)
	{
		double ret;
		ClbrPoint * rgh = FindNextDoorY(y);
		if(rgh != 0)                             //  point x not more rights than fin->x
		{
			if(rgh->y != y)                    //  point x  lefts that fin->x
			{
				if(rgh->prev != 0)          //  point x  is inside pointsList
				{
					double x0 = rgh->prev->x , x1 = rgh->x;
					double y0 = rgh->prev->y,  y1 = rgh->y;
					double dx = x1 - x0, dy = y1 - y0;
					if((double) abs(dy) > 0.005)
						ret = x0 + (y - y0) * dx/dy;
					else
						ret = y0;
				}
				else                   //    point x is not inside pointslist. It is lefts
					ret = rgh->x;					
			} 
			else                       //    x is one points of pointslist
				ret = rgh->x;
		}
		else if(fin != 0)           //     point x is outside of pointlist (more right) but poitslist is not empty
		{
			if(fin->prev != 0)    //     and have 2 or more points
			{
				double x0 = fin->prev->x , x1 = fin->x;
				double y0 = fin->prev->y,  y1 = fin->y;
				double dx = x1 - x0, dy = y1 - y0;
				if((double) abs(dy) > 0.005)
					ret = x1 + (y - y1) * dx/dy;
				else
					ret = x1;
			}
			else                   //         pointsList have 1 point == fin
				ret = fin->x;
		}
		else	                      //         pointslist is empty 		
			ret = 0.0;
		return ret;
	}
	#include <uc.h>
	using namespace uController;
	void Converter::AddPoint(double x, double y)
	{
		if(clbrList != 0)
		{
			ClbrPoint* tmp = FindNextDoor(x);
			if(tmp != 0)                              //x - inside current list
			{
				if(tmp->x != x)                     //point whith thise x not exist and we  create new point
				{
					ClbrPoint* newP = new ClbrPoint(x, y, tmp, tmp->prev);
					if(tmp->prev != 0)           //new point in interval between two existance points
						tmp->prev->next = newP;
					else                                //left point is absent
						clbrList = newP;         //and new point is lefts
					tmp->prev = newP;        //
				}
				else	
				{	//the point whith this x is present, we refresh y					
					tmp->y = y;
					PTg(1, 31);
				}
			}
			else  //x - rightes point
			{
				ClbrPoint* newP = new ClbrPoint(x, y, 0, fin);
				if(fin != 0 )
					fin->next = newP;
				fin = newP;
			}
		}
		else    //there are no points
		{
			clbrList = new ClbrPoint(x, y, 0, 0);
			fin = clbrList;
		}
	}
	
	void Converter::DeletePoint(ClbrPoint* p)
	{
		if(p->prev != 0)
			p->prev->next = p->next;
		else
			clbrList = p->next;
		if(p->next != 0)
			p->next->prev = p->prev;
		else
			fin = p->prev ;
		if(curr == p)
		{
			if(p->next != 0)
				curr = p->next;
			else if(p->prev != 0)
				curr = p->prev;
		}
		else
			curr = 0;
		delete p;
//		bool ret = false;
//		ClbrPoint* cur = GetPoint(x);
//		if(cur != 0)
//		{
//				if(cur->prev != 0)
//					cur->prev->next = cur->next;
//				if(cur->next != 0)
//					cur->next->prev = cur->prev;
//				delete cur;
//				ret = true;
//		}		
//		return ret;
	}
	
	void Converter::DeleteAll()
	{
		Erase();
	}

	void Converter::Erase()
	{
		ClbrPoint* cur = clbrList;
		while(cur != 0)
		{	
			delete clbrList;
			cur = cur->next;
			clbrList = cur;
		}
	}
	ClbrPoint*  Converter::GetFirstPoint()  const
	{
		return clbrList;		
	}
	
	ClbrPoint* Converter::GetLastPoint()  const
	{
		return fin;
	}
	
	ClbrPoint* Converter::GetPoint(double x) 
	{
		ClbrPoint* ret = 0;
		ClbrPoint* cur = clbrList;
		while((cur != 0) && (cur->x <  x))
			cur = cur->next;
		if(cur != 0)
		{
			ret = (cur->x == x)?  cur : 0;
		}
		return ret;	
	}
	
	ClbrPoint* Converter::FindNextDoor(double x)
	{
		ClbrPoint* cur = clbrList;
		while((cur != 0) && (cur->x <  x))
			cur = cur->next;
		return  cur;
	}
	
	ClbrPoint* Converter::FindNextDoorY(double y)
	{
		ClbrPoint* cur = clbrList;
		while((cur != 0) && (cur->y <  y))
			cur = cur->next;
		return  cur;
	}
	
	ClbrPoint* Converter::Next(ClbrPoint* p)  const
	{
		return (p == 0)? 0 : p->next;
	}
	
	ClbrPoint* Converter::Prev(ClbrPoint* p)  const
	{
		return (p == 0)? 0 : p->prev;
	}

	ClbrPoint* Converter::Curr() const 
	{
		return curr;
	}
	
	void Converter::SetCurr( ClbrPoint* p)
	{
		curr = p;
	}

	unsigned int Converter::GetNumPoints() 
	{
		unsigned int ret = 0;
		ClbrPoint* tmp = clbrList;
		while(tmp != 0)
		{
			tmp = tmp->next;
			++ret;
		}
		return ret;
	}


