#include "package.h"

namespace Tool{
	Package::Package(HeadType type,const char* buf,size_t buflen)
		:type(type)
	{
		data.append(buf,buflen);
	}

	Package::Package(HeadType type,char* buf,size_t buflen)
		:type(type)
	{
		data.append(buf,buflen);
	}

	void Package::initBuf(const char* buf,size_t buflen){
		data.initPos();
		data.append(buf,buflen);
	}
	size_t Package::getBuflen() const {
		return data.getPos()-type;
	}
	char *Package::getBuf() const {
		return data.getBuf()+type;
	}
	HeadType Package::getHeadType() const{
		return type;
	}
}
