#ifndef GLP_COUNTER_H_
#define GLP_COUNTER_H_
/*
	注释添加以及修改于 2014-4-2 

	提供一个计数器，每次获取一次计数的时候，自加1，不断循环
*/

namespace Tool{

class Mutex;
class Counter
{
public:
	Counter(int min = 1,int max = 100000000);
	virtual ~Counter();
	
	inline int get();

private:
	int m_count;
	int m_min;
	int m_max;

	Mutex* mutex;
};

}

#endif//GLP_COUNTER_H_
