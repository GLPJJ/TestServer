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
	/* package �����ݣ�
	������-HeadType�ֽڡ�����ʾ��-1�ֽڡ�|��cmd-4�ֽڡ���seq-4�ֽڡ���ver-2�ֽڡ���struct-�������ݡ�|
	 ǰ�治����							 |***---------�����ⲿ�ֶ����ڼ��ܲ���-------------------***|
	
	�ܵİ���С�� 65535
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
		����AES Key
		*/
		static void InitKey(const unsigned char* key,size_t keysize);
		static bool IsKeyInit(){return sInit;}
		static const unsigned char* GetKey();
	protected:
		//Ĭ��ʹ��AES���ܡ����ܣ����������������ʹ��ѹ���㷨��
		//���Ѿ���data�е����ݲ��ֽ��м���
		virtual void encode();
		//���Ѿ���data�е����ݲ��ֽ��н���
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
