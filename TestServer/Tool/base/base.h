#ifndef GLP_BASE_H_
#define GLP_BASE_H_

#define TOOL_EVENT_10_SEC   10000
#define TOOL_EVENT_INFINITE 0xffffffff

// Object that will be passed by the spawned thread when it enters the callback
// function.
#define ThreadObj void*

#define DISALLOW_COPY_AND_ASSIGN(M_TypeName) \
M_TypeName(const M_TypeName&); \
void operator=(const M_TypeName&)


//�����������ͣ�
typedef enum _StreamType
{
	PROTOCOLTYPE_TEXT = 1,//�ַ���
	PROTOCOLTYPE_BINARY,//2��������
}StreamType;
typedef enum _HeadType
{
	//��������ȵ�ö������
	HEADER_LEN_2 = 2,
	HEADER_LEN_4 = 4
}HeadType;

#define PACKAGE_MAXLEN	 0xffff//65535

#endif//GLP_BASE_H_
