#ifndef TESTCLIENTSOCKET_H_
#define TESTCLIENTSOCKET_H_

#define GLP_SERVER 1

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

class TestListenSocket : public Tool::ListenSocketBase2
{
public:
	TestListenSocket(int port,Tool::Reactor *pReactor,Tool::DataProcessBase*pDecoder) 
		: Tool::ListenSocketBase2(port,pReactor,pDecoder) 
	{}


	void TestListenSocket::onAccept(int fd);

protected:
	void dealErrClient(Tool::ClientSocketBase* client);

public:
	void sendToClient(int pos,Tool::WriteStream& ws){
		if(pos>fds.size())
			return;

		std::list<int>::iterator it = fds.begin();
		int i = 0;
		while(i<pos){
			it ++;
			i ++;
		}

		Tool::ClientSocketBase** p = m_pClientMap.get(*it);
		if(p && *p){
			(*p)->addBuf(ws);
		}
	}

private:
	std::list<int> fds;
};

#endif//TESTCLIENTSOCKET_H_
