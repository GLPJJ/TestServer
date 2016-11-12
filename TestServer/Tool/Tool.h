#ifndef GLP_TOOL_H_
#define GLP_TOOL_H_

#include "base/config.h"

#include "base/log.h"
#include "base/counter.h"
#include "base/datablock.h"
#include "base/file.h"
#include "base/object.h"
#include "base/mutex.h"
#include "base/event.h"
#include "base/thread.h"
#include "base/thread_mgr.h"
#include "base/timer.h"
#include "base/json.h"
#include "base/rwstream.h"

#include "base/dataprocess.h"
#include "base/eventhandler.h"
#include "base/reactor.h"
#include "base/clientsocket.h"

#include "base/httpdownload.h"
#include "base/httpdownloadmgr.h"
#include "base/httpcontent.h"

#include "base/serversocket.h"
#include "base/package.h"

#include "algorithms/algorithms.h"
#include "algorithms/crypt.h"

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
