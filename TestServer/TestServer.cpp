// TestServer.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
//#include "vld.h"
#include "TestServer.h"
#include "TestServerDlg.h"
#include "TestClientSocket.h"

//ȥ�����DEBUG_NEW
// #ifdef _DEBUG
// #define new DEBUG_NEW
// #endif


// CTestServerApp

BEGIN_MESSAGE_MAP(CTestServerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTestServerApp ����

CTestServerApp::CTestServerApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CTestServerApp ����

CTestServerApp theApp;
HANDLE g_hStdOut = NULL;

void NewConsole()
{
	AllocConsole();
	g_hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
}

#include "Tool/Tool.h"

using namespace Tool;

class TestCls{
public:
	int a;

	TestCls():a(){
		Log("TestCls() , a=%d\n",a);
	}
	~TestCls(){
		Log("~TestCls()\n");
	}
};

class TestCls1{
public:
	int a;

	TestCls1(int a):a(a){
		Log("TestCls1() , a=%d\n",a);
	}
	~TestCls1(){
		Log("~TestCls1()\n");
	}
};

#include "winsock2.h"

static int count = 0;
class LogTimer : public TMEventHandler
{
public:
	LogTimer(Reactor* reactor):TMEventHandler(reactor){}

	virtual void onTimeOut()
	{
		int ys = count % 3;
		if(ys == 0)
			Log("\n\r�����ͻ������ӡ�");
		else if(ys == 1)
			Log("\r�����ͻ������ӡ���");
		else if(ys == 2)
			Log("\r�����ͻ������ӡ�����");
		count ++;
	}
};

extern CComboBox* pCCBox;


void TestListenSocket::onAccept(int fd){
	if(m_pClientMap.get(fd) == NULL){
		ListenSocketBase2::onAccept(fd);

		fds.push_back(fd);

		char name[100] = {0};
		sprintf(name,"%d",fd);
		pCCBox->AddString(name);
	}
}
void TestListenSocket::dealErrClient(ClientSocketBase* client){
	ListenSocketBase2::dealErrClient(client);

	std::list<int>::iterator it = fds.begin();

	int pos = 0;
	//pCCBox->ResetContent();
	pCCBox->SetCurSel(-1);
	while(it != fds.end()){
		if(*it == client->getFD()){
			pCCBox->DeleteString(pos);
			fds.erase(it);
			break;
		}
		it ++;
		pos ++;
	}
}

class ServerDataProcess : public DataProcess
{
public:
	ServerDataProcess(StreamType pttype,HeadType hdlen)
		:DataProcess(pttype,hdlen){}

	virtual int onPackage(ClientSocketBase *pClient,Package* package)
	{
		BinaryReadStream brs(package);
		
		char buf[100] = {0};
		size_t len = sizeof(buf);
		brs.read(buf,len,len);
		Log("�յ�����(%s)������[len=%d] : %s\n",pClient->getPeerIp(),len,buf);

		const char* replay = "���ð��������Ƿ�����~!";
		Log("���͸�(%s):%s\n",pClient->getPeerIp(),replay);

		BinaryWriteStreamT<> ws;
		ws.write(replay,strlen(replay));
		ws.flush();
		pClient->addBuf(ws);

		return 0;
	}
};

static NetServerReactor serverReactor;
TestListenSocket* pServerSocket;
bool ServerSocketFunc(ThreadObj obj)
{
	WSADATA wsaData;

	//-----------------------------------------------
	// Initialize Winsock
	WSAStartup(MAKEWORD(2,2), &wsaData);

	ServerDataProcess process(PROTOCOLTYPE_BINARY,HEADER_LEN_2);
	TestListenSocket listenSock(9800,&serverReactor,&process);
	pServerSocket = &listenSock;
	listenSock.listen();
	LogTimer logTimer(&serverReactor);
	logTimer.registerTimer(3);

	serverReactor.run();

	WSACleanup();

	return false;
}

// ���ӳɹ�
bool TestClientSocket::onSocketConnect() {
	Log("%s\n",__FUNCTION__);

	/*
	char sendbuf[] = "��ã�����������";

	BinaryWriteStreamT<> bws;
	bws.write(sendbuf,sizeof(sendbuf));
	bws.flush();

	Log("send server data(len=%d):%s\n",sizeof(sendbuf),sendbuf);
	sendBuf(bws);
	*/
	return true;
} 
// ���ӳ�ʱ
void TestClientSocket::onSocketConnectTimeout() {
	Log("%s\n",__FUNCTION__);
}
// �����ر�(�����ر�),recv == 0�����
void TestClientSocket::onSocketClose() {
	Log("%s\n",__FUNCTION__);
}
// errcodeΪ������(socket�ṩ)
void TestClientSocket::onSocketConnectError(int errCode) {
	Log("%s,error code = %d\n",__FUNCTION__,errCode);
}
void TestClientSocket::onSocketRecvError(int errCode) {
	Log("%s,error code = %d\n",__FUNCTION__,errCode);
}
void TestClientSocket::onSocketSendError(int errCode) {
	Log("%s,error code = %d\n",__FUNCTION__,errCode);
}
// ��������(errCode����㶨��)
void TestClientSocket::onNetLevelError(int errCode) {
	Log("%s,error code = %d\n",__FUNCTION__,errCode);
}


class ClientDataProcess : public DataProcess
{
public:
	ClientDataProcess(StreamType pttype,HeadType hdlen)
		:DataProcess(pttype,hdlen){}

	virtual int onPackage(ClientSocketBase *pClient,Package* package)
	{
		BinaryReadStream brs(package);

		char buf[1024] = {0};
		size_t len = sizeof(buf);
		brs.read(buf,len,len);
		Log("�յ����Է�����������[len=%d] : %s\n",len,buf);

		return 0;
	}
};

static NetClientReactor clientReactor;
TestClientSocket clientSock(&clientReactor);
TestClientSocket* pClientSock = &clientSock;
bool ClientSocketFunc(ThreadObj obj)
{
	WSADATA wsaData;

	//-----------------------------------------------
	// Initialize Winsock
	WSAStartup(MAKEWORD(2,2), &wsaData);
	
	ClientDataProcess process(PROTOCOLTYPE_BINARY,HEADER_LEN_2);
	clientSock.setDecoder(&process);
	clientSock.connect("127.0.0.1",9800);

	clientReactor.run();

	WSACleanup();

	return false;
}

// CTestServerApp ��ʼ��

BOOL CTestServerApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	
	NewConsole();

	char plaintext[] = "12345678";
	Log("����ǰ : %s\n",plaintext);
	//test MD5
	std::string md5 = Algorithms::EncodeWithMd5((unsigned char*)plaintext,strlen(plaintext));
	Log("md5 : ");
	LogCiphertext((const unsigned char*)md5.c_str(),md5.length());
	LogN();
	//test AES
	unsigned char outbuffer[200] = {0};
	unsigned char outbuffer2[200] = {0};
	unsigned char key[32] = {0x60,0x3d,0xeb,0x10,0x15,0xca,0x71,0xbe,0x2b,0x73,0xae,0xf0,0x85,0x7d,0x77,0x81,0x1f,0x35,0x2c,0x07,0x3b,0x61,0x08,0xd7,0x2d,0x98,0x10,0xa3,0x09,0x14,0xdf,0xf4};
	Algorithms::EncodeWithAesCtr(key,256,(unsigned char*)plaintext,strlen(plaintext),outbuffer);
	Log("aes ciphertext : ");
	LogCiphertext(outbuffer,strlen((const char*)outbuffer));
	LogN();
	Algorithms::DecodeWithAesCtr(key,256,outbuffer,strlen(plaintext),outbuffer2);
	Log("aes plaintext : %s\n",outbuffer2);

	Log("I'm GLP\n");

	TestCls* pT1 = (TestCls*)calloc_(sizeof(TestCls));
	free_(pT1);
	//pT1->TestCls();//���캯����ʹ�ò���������
	Log("***************** 1 *******************\n");
 	new_(TestCls,pT2);
  	new_(TestCls1,pT3,1);
  
  	delete_(TestCls,pT2);
  	delete_(TestCls1,pT3);
	Log("***************** 2 *******************\n");

	size_t tid;//11
#if GLP_SERVER
	Thread* server = Thread::CreateThread(ServerSocketFunc,this);
	server->start(tid);
#else
	Thread* client = Thread::CreateThread(ClientSocketFunc,this);
	client->start(tid);
#endif

	CTestServerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˴����ô����ʱ�á�ȷ�������ر�
		//  �Ի���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ�á�ȡ�������ر�
		//  �Ի���Ĵ���
	}

	serverReactor.stop();
	clientReactor.stop();

#if GLP_SERVER
	server->stop();
	Thread::Destroy(server);
#else
	client->stop();
	Thread::Destroy(client);
#endif

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
