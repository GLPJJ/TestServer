#ifndef GLP_COUNTER_H_
#define GLP_COUNTER_H_

namespace Tool{

class Mutex;
class Counter
{
public:
	Counter(int min = 1,int max = 100000000);
	virtual ~Counter();
	
	inline int get();

	static int Get();
private:
	int m_count;
	int m_min;
	int m_max;

	Mutex* mutex;
};

}

#endif//GLP_COUNTER_H_
