#ifndef ALGORITHMS__H__
#define ALGORITHMS__H__

#include <string>

namespace Tool{

class Algorithms {
	
public:

	/*
	@param file:ָ����Ҫ���ܵ��ļ�

	*/
	static std::string EncodeWithMd5(const char* file);
	/*
	@param data:ָ������
	@param len:ָ�����ݳ���

	//�������ݼ�����Ҫ utf8��ʽ
	*/
	static std::string EncodeWithMd5(const unsigned char* data,size_t len);

	/*
	AES����

	@param key:ָ��key
	@param keysize:ָ��key��λ��
	*/
	static bool EncodeWithAesCtr(const unsigned char* key,size_t keysize,const unsigned char* plaintext,size_t len,unsigned char* out);
	/*
	AES����

	@param key:ָ��key
	@param keysize:ָ��key��λ��
	*/
	static bool DecodeWithAesCtr(const unsigned char* key,size_t keysize,const unsigned char* ciphertext,size_t len,unsigned char* out);

};

}

#endif//ALGORITHMS__H__