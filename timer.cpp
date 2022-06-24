#include "timer.h"


TimerK::TimerK()
{ LARGE_INTEGER *freq = new LARGE_INTEGER;

  QueryPerformanceFrequency(freq);   // current frequency

  frequency = freq->QuadPart;
  freq_ms = frequency /1000;
  freq_add= (double)(frequency%1000)/1000;
  SetZero();
  
  delete freq;
}


TimerK::~TimerK()
{  
}



void TimerK::SetZero()
{
  LARGE_INTEGER *startTime_int = new LARGE_INTEGER;

  QueryPerformanceCounter(startTime_int);
  star=startTime_int->QuadPart;
  curr_old = star;
  star_plus=0;
  ms_count=0;
  add_count=0.0;
  curr_plus=0;
  
  delete startTime_int;
}



long int TimerK::GetMS()
{  _int64 temp;
   LARGE_INTEGER *curr = new LARGE_INTEGER;

   QueryPerformanceCounter(curr);
   current=curr->QuadPart;
   
   if (current<curr_old) curr_plus++;

   while (star<current || star_plus<curr_plus)
   {temp=star+freq_ms;
	if (temp<star) 
		star_plus++;
	add_count+=freq_add;
	if ((int)(add_count))
	{temp+=(int)(add_count);add_count-=(float)((int)(add_count));}
	star = temp;
	ms_count++;
   }
   curr_old=current;
   delete curr;
   return ms_count;
}
