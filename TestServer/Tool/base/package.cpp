#include "package.h"
#include <string.h>

#define COMPRESS_SIZE 256

namespace Tool{

	unsigned char Package::sKey[32] = {0};
	size_t Package::sKeySize = 0;
	bool Package::sInit = false;

	void Package::InitKey(const unsigned char* key,size_t keysize)
	{
		if(!key)
			return;

		if(keysize == 128 || keysize ==192 || keysize ==256){
			memcpy(sKey,key,keysize/8);
			sKeySize = keysize;
			sInit = true;
		} 
	}
	const unsigned char* Package::GetKey()
	{
		return sKey;
	}

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

	//默认使用AES加密、解密
	void Package::encode(){
		if(data.getPos() > COMPRESS_SIZE){
			
		}
	}
	void Package::decode(){
		
	}
}
