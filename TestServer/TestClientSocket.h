#ifndef TESTCLIENTSOCKET_H_
#define TESTCLIENTSOCKET_H_

#include "Tool/Tool.h"

class TestClientSocket : public Tool::ClientSocket
{
public:
	TestClientSocket(Tool::Reactor *pReactor):ClientSocket(pReactor){}

	// 连接成功
	virtual bool onSocketConnect();
	// 连接超时
	virtual void onSocketConnectTimeout();
	// 正常关闭(被动关闭),recv == 0的情况
	virtual void onSocketClose();
	// errcode为错误码(socket提供)
	virtual void onSocketConnectError(int errCode);
	virtual void onSocketRecvError(int errCode);
	virtual void onSocketSendError(int errCode);
	// 网络层错误(errCode网络层定义)
	virtual void onNetLevelError(int errCode);
};

#endif//TESTCLIENTSOCKET_H_
