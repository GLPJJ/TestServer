#include "object.h"
#include <malloc.h>
#include <assert.h>

namespace Tool{

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

	// Allocator
	void* Allocator::allocate(unsigned int size){
		void* pointer = malloc(size);
		return pointer;
	}
	void Allocator::deallocate(void* pointer){
		if (pointer != NULL)
			free(pointer);
	}

	Allocator* Allocator::GetInstance(){
		static Allocator ins;
		return &ins;
	}

}
