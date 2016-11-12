#include "package.h"

namespace Tool{
	Package::Package(const char* buf,size_t buflen){
		data.append(buf,buflen);
	}

	void Package::initBuf(const char* buf,size_t buflen){
		data.initPos();
		data.append(buf,buflen);
	}
	size_t Package::getBuflen() const {
		return data.getPos();
	}
	char *Package::getBuf() const {
		return data.getBuf();
	}
}
