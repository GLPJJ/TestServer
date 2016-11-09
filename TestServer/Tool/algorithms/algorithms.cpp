#include "algorithms.h"
#include <stdio.h>

#include "wrap/md5.h"

namespace Tool{
	std::string Algorithms::EncodeWithMd5(const char* file){
		if(file)
			return "";

		FILE* fp = fopen(file,"rb");
		if(!fp)
			return "";

		unsigned char buf[16] = {0};

		MD5_CTX ctx;
		md5_init(&ctx);

		unsigned char buffer[1024];
		int lenRead;
		while((lenRead=(int)fread(buffer,1,1024,fp))  > 0){
			md5_update(&ctx, buffer, lenRead);
		}
		md5_final(&ctx, buf);

		fclose(fp);

		return std::string((char*)buf);
	}

	std::string Algorithms::EncodeWithMd5(const unsigned char* data,unsigned int len){

		unsigned char buf[16] = {0};

		MD5_CTX ctx;
		md5_init(&ctx);
		md5_update(&ctx, data, len);
		md5_final(&ctx, buf);

		return std::string((char*)buf);
	}
}


