#ifndef GLP_OBJECT_H_
#define GLP_OBJECT_H_

#include <string.h>

namespace Tool{

	class Object{
	private:
		Object(const char* name=NULL);
		virtual ~Object();

	public:
		int hold();
		int release();

	private:
		char name[200];
		int ref;
	};

	class Allocator{
	private:
		virtual ~Allocator(){}

	public:
		/*
		@isize  期望分配的内存大小
		*/
		virtual void* allocate(unsigned int size);
		virtual void deallocate(void* pointer);

		static Allocator* GetInstance();
	};

	//经常复用的对象需要使用 New和Delete
	template<typename T>
	T* New(unsigned int size){
		void* p = Allocator::GetInstance()->allocate(size);
		if (p != NULL)
			return new (p) T;
		else
			return NULL;
	}

	template<typename T>
	void Delete(T* pointer){
		if (pointer != NULL)
		{
			pointer->~T();
			Allocator::GetInstance()->deallocate(pointer);
		}
	}

#define new_(M_T,M_o,...) \
	M_T* M_o = Allocator::GetInstance()->allocate(sizeof(M_T)); \
	M_o = M_o == NULL ? NULL : new (M_o)T(__VA_ARGS__)

#define delete_(M_o)\
	if (M_o != NULL){\
		M_o->~T();\
		Allocator::GetInstance()->deallocate(M_o);\
	}
}

#endif//GLP_OBJECT_H_
