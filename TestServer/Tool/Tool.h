#ifndef GLP_TOOL_H_
#define GLP_TOOL_H_

#include "base/log.h"
#include "base/object.h"
#include "base/mutex.h"
#include "base/counter.h"

namespace Tool{

	/*
	class VoidGuard
	{
	public:
		VoidGuard(void *p) : m_p(p){}
		virtual ~VoidGuard(){if(m_p)free(m_p);}

	private:
		void *m_p;
	};

	template<typename Cls>
	class SafePointer
	{
	public:
		SafePointer(Cls* p):m_p(p){}
		~SafePointer(){if(m_p)delete m_p;}
	private:
		Cls* m_p;
	};
	*/

}

#endif//GLP_TOOL_H_
