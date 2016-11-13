#ifndef GLP_DATADECODER_H_
#define GLP_DATADECODER_H_

#include <stdio.h>

//定义 包解析的接口
namespace Tool
{
	class ClientSocketBase;
	class HttpSocketBase;
	class Package;

	//流的数据类型，
	typedef enum _StreamType
	{
		PROTOCOLTYPE_TEXT = 1,//字符串
		PROTOCOLTYPE_BINARY,//2进制数据
	}StreamType;
	typedef enum _HeadType
	{
		//定义包长度的枚举类型
		HEADER_LEN_2 = 2,
		HEADER_LEN_4 = 4,
		HEADER_LEN_6 = 6,
	}HeadType;

	class DataProcessBase
	{
	public:
		virtual ~DataProcessBase(){}

		virtual int process(ClientSocketBase *pClient) = 0;
	};

	class DataProcess : public DataProcessBase
	{
	public:
		DataProcess(StreamType pttype,HeadType hdlen) : m_pttype(pttype),m_hdlen(hdlen) {}
		virtual ~DataProcess(){}
		//解析分发完整包数据,交给OnPackage处理
		virtual int process(ClientSocketBase *pClient);
		//对收到的包进行处理： 返回0成功，返回非0失败
		virtual int onPackage(ClientSocketBase *pClient,Package* package) = 0;
		//从提供的参数buf中获取包的长度 Net -> Loacal
		size_t getBuflen(char *buf);

	protected:
		StreamType	m_pttype;
		HeadType		m_hdlen;
	};
}
#endif//DATADECODER__H__
