// TestServer.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "TestServer.h"
#include "TestServerDlg.h"

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
			Log("\r�����ͻ������ӡ�");
		else if(ys == 1)
			Log("\r�����ͻ������ӡ���");
		else if(ys == 2)
			Log("\r�����ͻ������ӡ�����");
		count ++;
	}
};


bool ServerSocketFunc(ThreadObj obj)
{
	WSADATA wsaData;

	//-----------------------------------------------
	// Initialize Winsock
	WSAStartup(MAKEWORD(2,2), &wsaData);

	NetServerReactor reactor;
	ListenSocketBase2 listenSock(9800,&reactor);
	listenSock.listen();
	LogTimer logTimer(&reactor);
	logTimer.registerTimer(1);

	reactor.run();

	WSACleanup();

	return false;
}

bool ClientSocketFunc(ThreadObj obj)
{
	WSADATA wsaData;

	//-----------------------------------------------
	// Initialize Winsock
	WSAStartup(MAKEWORD(2,2), &wsaData);

	NetClientReactor reactor;
	ClientSocket clientSock(&reactor);
	clientSock.connect("127.0.0.1",9800);

	reactor.run();

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

	unsigned int tid;//11
	Thread* server = Thread::CreateThread(ServerSocketFunc,this);
	server->start(tid);

// 	Thread* client = Thread::CreateThread(ClientSocketFunc,this);
// 	client->start(tid);

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

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
