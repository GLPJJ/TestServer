#ifndef GLP_PACKAGE_H_
#define GLP_PACKAGE_H_

#include "base.h"
#include "datablock.h"

namespace Tool{

	class ClientId{
	public:
		ClientId():m_clientid(0){}

		void setClientID(int id){m_clientid = id;}
		int getClientID() const {return m_clientid;}
	private:
		int m_clientid;
	};

	/************************************************************************/
	/* package 包内容：
	【包长-HeadType字节】【表示符-1字节】|【cmd-4字节】【seq-4字节】【ver-2字节】【struct-数据内容】|
	 前面不加密							 |***---------后面这部分都属于加密部分-------------------***|
	
	总的包长小余 65535
	*/
	/************************************************************************/
	class Package : public ClientId
	{
		DISALLOW_COPY_AND_ASSIGN(Package);
	public: 
		Package(HeadType type,const char* buf,size_t buflen);
		Package(HeadType type,char* buf,size_t buflen);

		void initBuf(const char* buf,size_t buflen);
		size_t getBuflen() const;
		char *getBuf() const;
		HeadType getHeadType() const;

	public:
		/*
		设置AES Key
		*/
		static void InitKey(const unsigned char* key,size_t keysize);
		static bool IsKeyInit(){return sInit;}
		static const unsigned char* GetKey();
	protected:
		//默认使用AES加密、解密，如果数据量过大则使用压缩算法。
		//对已经在data中的数据部分进行加密
		virtual void encode();
		//对已经在data中的数据部分进行解密
		virtual void decode();
	private:
		HeadType type;
		DataBlock data;

		static unsigned char sKey[32];
		static size_t sKeySize;
		static bool sInit;
	};

}


#endif//GLP_PACKAGE_H_
