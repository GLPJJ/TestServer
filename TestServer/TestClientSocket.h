#ifndef TESTCLIENTSOCKET_H_
#define TESTCLIENTSOCKET_H_

#define GLP_SERVER 1

#include "Tool/Tool.h"

class TestClientSocket : public Tool::ClientSocket
{
public:
	TestClientSocket(Tool::Reactor *pReactor):ClientSocket(pReactor){}

	// ���ӳɹ�
	virtual bool onSocketConnect();
	// ���ӳ�ʱ
	virtual void onSocketConnectTimeout();
	// �����ر�(�����ر�),recv == 0�����
	virtual void onSocketClose();
	// errcodeΪ������(socket�ṩ)
	virtual void onSocketConnectError(int errCode);
	virtual void onSocketRecvError(int errCode);
	virtual void onSocketSendError(int errCode);
	// ��������(errCode����㶨��)
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
