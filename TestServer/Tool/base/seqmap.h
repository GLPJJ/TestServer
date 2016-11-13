#ifndef GLP_SEQMAP__H__
#define GLP_SEQMAP__H__

#include <map>
#include <assert.h>

class Mutex;
namespace Tool
{
	//template<typename T = char,int len=10>
	template <class T,class Map = std::map<int,T> >
	class SeqMap
	{
	public:
		typedef SeqMap<T, Map> _Myt;
		typedef T seq_type;
		typedef Map seq_map_type;
		
	public:
		virtual ~SeqMap(){}

		bool put(int id,T t)
		{
			if(m_clientmap.find(id)==m_clientmap.end())
			{
				m_clientmap.insert(std::make_pair(id,t));
				return true;
			}
			return false;
		}
		void cover(int id,T t){m_clientmap[id] = t;}
		T* get(int id)
		{
			typename Map::iterator it;
			it = m_clientmap.find(id);
			if(it == m_clientmap.end())
				return NULL;
			return &(it->second);
		}
		void del (int id) {m_clientmap.erase(id);}
		unsigned int size() {return m_clientmap.size();}
		void clear() { m_clientmap.clear();}

		seq_map_type* getMap(){return &m_clientmap;}
	private:
		seq_map_type m_clientmap;
	};
}

#endif//GLP_SEQMAP__H__
