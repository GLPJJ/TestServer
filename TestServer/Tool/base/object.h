#ifndef GLP_OBJECT_H_
#define GLP_OBJECT_H_

#include "log.h"
#include <string.h>
#include <list>

/*
�ڴ�أ���������������
*/


//�ɵ�vc�����new �Ķ���
//#undef new

namespace Tool{
	class Mutex;
	class Object;

	typedef std::list<Object*> LISTOBJS;
	//������������
	class ObjectMgr{
	private:
		ObjectMgr();
		virtual ~ObjectMgr();
	public:
		static ObjectMgr* GetInstance();

		void addObj(Object* obj);
		void removeObj(Object* obj);

		void clearObjs();

	private:
		LISTOBJS objs;
		Mutex* mutex;
	};

	//�����������̳�Object
	class Object{
	private:
		Object(const char* name=NULL);
		virtual ~Object();

	public:
		/*holdס
		@return ref;
		*/
		int hold();
		/*�ͷ� 
		@return ref;
		*/
		int release();

		//�йܸ�ObjectMgr
		void hosting();

	private:
		char name[200];
		int ref;
		ObjectMgr* mgr;
	};

	//�ڴ���������
	class Allocator{
	private:
		virtual ~Allocator(){}

	public:
		/*
		@size  ����������ڴ��С
		*/
		virtual void* alloc(unsigned int size);
		virtual void dealloc(void* pointer);

		static Allocator* GetInstance();
	};
}


//ͨ�ö���� new_��delete_�����Բ��ü̳�Object
#define new_(M_T,M_o,...) \
	M_T* M_o = (M_T*)Tool::Allocator::GetInstance()->alloc(sizeof(M_T)); \
	M_o = M_o == NULL ? NULL : new (M_o)M_T(__VA_ARGS__)

#define delete_(M_T,M_o)\
	if (M_o != NULL){\
	M_o->~M_T();\
	Tool::Allocator::GetInstance()->dealloc(M_o);\
	}

#endif//GLP_OBJECT_H_
