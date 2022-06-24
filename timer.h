#ifndef TIMERK
#define TIMERK

#include <time.h>
#include <windows.h>



class TimerK {
public:
	TimerK();
	~TimerK();
	void SetZero(void);
	long int GetMS(void);

private:
    _int64 current, 
           star, 
           frequency, 
           freq_ms, 
           curr_old;
    long int ms_count, 
             star_plus, 
             curr_plus;
    double add_count, 
           freq_add;
};

#endif // TIMERK