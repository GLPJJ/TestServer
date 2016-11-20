#ifndef TESTCLIENTSOCKET_H_
#define TESTCLIENTSOCKET_H_

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

#endif//TESTCLIENTSOCKET_H_
