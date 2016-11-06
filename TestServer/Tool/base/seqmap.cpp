#include "seqmap.h"

namespace Tool
{
	template <class T,class Map>
	void SeqMap<T,Map>::Put(int id,T t) {
		m_clientmap.insert(std::make_pair(id,t));
	}

	template <class T,class Map>
	void SeqMap<T,Map>::Cover(int id,T t) {
		m_clientmap[id] = t;
	}

	template <class T,class Map>
	T* SeqMap<T,Map>::Get(int id) {
		typename _MapT::iterator it;
		it = m_clientmap.find(id);
		if(it == m_clientmap.end())
			return NULL;
		return &(it->second);
	}

	template <class T,class Map>
	void SeqMap<T,Map>::Del (int id) {
		m_clientmap.erase(id);
	}

	template <class T,class Map>
	unsigned int SeqMap<T,Map>::Size() {
		return m_clientmap.size();
	}

	template <class T,class Map>
	void SeqMap<T,Map>::Clear() { 
		m_clientmap.clear();
	}
}
