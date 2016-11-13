#include "config.h"
#include "object.h"
#include "mutex.h"
#include <malloc.h>
#include <assert.h>
#include <string.h>

namespace Tool{

	ObjectMgr::ObjectMgr(){
		mutex = Mutex::CreateCriticalSection();
		assert(mutex);
	}
	ObjectMgr::~ObjectMgr(){
		delete_(Mutex,mutex);
	}

	ObjectMgr* ObjectMgr::GetInstance(){
		static ObjectMgr ins;
		return &ins;
	}

	void ObjectMgr::addObj(Object* obj){
		if(obj == NULL)
			return;

		MutexScoped lock(mutex);
		obj->hold();
		objs.push_back(obj);
	}
	void ObjectMgr::removeObj(Object* obj){
		if(obj == NULL)
			return;

		MutexScoped lock(mutex);
		objs.remove(obj);
		obj->release();
	}

	//需要单元测试下
	void ObjectMgr::clearObjs(){
		MutexScoped lock(mutex);
		LISTOBJS::iterator it = objs.begin();
		while(it!=objs.end()){
			(*it)->release();
			it = objs.erase(it);
		}
	}

	Object::Object(const char* name)
		:ref(1)
	{
		if (name != NULL)
			strcpy(this->name, name);
		else
			this->name[0] = 0;
	}
	Object::~Object(){

	}

	int Object::hold(){
		return ++ref;
	}
	int Object::release(){
		assert(ref > 0);

		ref--;
		if (ref == 0){
			delete this;
		}
		return ref;
	}

	void Object::hosting(){
		//ObjectMgr::GetInstance()->addObj(this);
		release();
	}

	// Allocator
	void* Allocator::alloc(size_t size){
		void* pointer = calloc(1,size);
		Log("alloc mem pointer = 0x%08x,size = %d\n",(TPOINTER)pointer,size);
		return pointer;
	}
	void Allocator::dealloc(void* pointer){
		if (pointer != NULL){
			Log("dealloc mem pointer = 0x%08x\n",(TPOINTER)pointer);
			free(pointer);
		}
	}

	Allocator* Allocator::GetInstance(){
		static Allocator ins;
		return &ins;
	}

}
