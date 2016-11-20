#include "algorithms.h"
#include "../base/log.h"
#include "../base/object.h"
#include <stdio.h>

extern "C" {//c文件引入
#include "wrap/md5.h"
#include "wrap/aes.h"
}


namespace Tool{
	std::string Algorithms::EncodeWithMd5(const char* file){
		if(file)
			return "";

		FILE* fp = fopen(file,"rb");
		if(!fp)
			return "";

		unsigned char buf[17] = {0};

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

	std::string Algorithms::EncodeWithMd5(const unsigned char* data,size_t len){

		unsigned char buf[17] = {0};

		MD5_CTX ctx;
		md5_init(&ctx);
		md5_update(&ctx, data, len);
		md5_final(&ctx, buf);

		return std::string((char*)buf);
	}

	bool Algorithms::EncodeWithAesCtr(const unsigned char* key,size_t keysize,const unsigned char* plaintext
		,size_t len,unsigned char* out)
	{
		if(!key || !plaintext || !out )
			return false;
		if(keysize == 128 || keysize ==192 || keysize ==256)
		{
			WORD key_schedule[60];
			BYTE iv[1][16] = {
				{0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff},
			};
			aes_key_setup(key, key_schedule, keysize);
			aes_encrypt_ctr(plaintext, len, out, key_schedule, keysize, iv[0]);
			return true;
		}
		return false;
	}
	bool Algorithms::DecodeWithAesCtr(const unsigned char* key,size_t keysize,const unsigned char* ciphertext
		,size_t len,unsigned char* out)
	{
		if(!key || !ciphertext || !out )
			return false;
		if(keysize == 128 || keysize ==192 || keysize ==256)
		{
			WORD key_schedule[60];
			BYTE iv[1][16] = {
				{0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff},
			};
			aes_key_setup(key, key_schedule, keysize);
			aes_decrypt_ctr(ciphertext, len, out, key_schedule, keysize, iv[0]);
			return true;
		}
		return false;
	}
}


