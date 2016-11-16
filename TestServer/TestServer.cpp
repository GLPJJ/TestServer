// TestServer.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "vld.h"
#include "TestServer.h"
#include "TestServerDlg.h"

//去他娘的DEBUG_NEW
// #ifdef _DEBUG
// #define new DEBUG_NEW
// #endif


// CTestServerApp

BEGIN_MESSAGE_MAP(CTestServerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTestServerApp 构造

CTestServerApp::CTestServerApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CTestServerApp 对象

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
			Log("\n\r监听客户端连接。");
		else if(ys == 1)
			Log("\r监听客户端连接。。");
		else if(ys == 2)
			Log("\r监听客户端连接。。。");
		count ++;
	}
};

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
		Log("收到来自(%s)的内容[len=%d] : %s\n",pClient->getPeerIp(),len,buf);

		return 0;
	}
};

static NetServerReactor serverReactor;
bool ServerSocketFunc(ThreadObj obj)
{
	WSADATA wsaData;

	//-----------------------------------------------
	// Initialize Winsock
	WSAStartup(MAKEWORD(2,2), &wsaData);

	ServerDataProcess process(PROTOCOLTYPE_BINARY,HEADER_LEN_2);
	ListenSocketBase2 listenSock(9800,&serverReactor,&process);
	listenSock.listen();
	LogTimer logTimer(&serverReactor);
	logTimer.registerTimer(3);

	serverReactor.run();

	WSACleanup();

	return false;
}

class TestClientSocket : public ClientSocket
{
public:
	TestClientSocket(Reactor *pReactor):ClientSocket(pReactor){}

	// 连接成功
	virtual bool onSocketConnect() {
		Log("%s\n",__FUNCTION__);

		char sendbuf[] = "你好，我是张欣怡";

		BinaryWriteStreamT<> bws;
		bws.write(sendbuf,sizeof(sendbuf));
		bws.flush();

		Log("send server data(len=%d):%s\n",sizeof(sendbuf),sendbuf);
		sendBuf(bws);
		return true;
	} 
	// 连接超时
	virtual void onSocketConnectTimeout() {
		Log("%s\n",__FUNCTION__);
	}
	// 正常关闭(被动关闭),recv == 0的情况
	virtual void onSocketClose() {
		Log("%s\n",__FUNCTION__);
	}
	// errcode为错误码(socket提供)
	virtual void onSocketConnectError(int errCode) {
		Log("%s,error code = %d\n",__FUNCTION__,errCode);
	}
	virtual void onSocketRecvError(int errCode) {
		Log("%s,error code = %d\n",__FUNCTION__,errCode);
	}
	virtual void onSocketSendError(int errCode) {
		Log("%s,error code = %d\n",__FUNCTION__,errCode);
	}
	// 网络层错误(errCode网络层定义)
	virtual void onNetLevelError(int errCode) {
		Log("%s,error code = %d\n",__FUNCTION__,errCode);
	}
};

static NetClientReactor clientReactor;
TestClientSocket clientSock(&clientReactor);
bool ClientSocketFunc(ThreadObj obj)
{
	WSADATA wsaData;

	//-----------------------------------------------
	// Initialize Winsock
	WSAStartup(MAKEWORD(2,2), &wsaData);

	clientSock.connect("127.0.0.1",9800);

	clientReactor.run();

	WSACleanup();

	return false;
}

#define GLP_SERVER 1

// CTestServerApp 初始化

BOOL CTestServerApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	
	NewConsole();

	Log("I'm GLP\n");

	TestCls* pT1 = (TestCls*)calloc_(sizeof(TestCls));
	free_(pT1);
	//pT1->TestCls();//构造函数的使用不可以这样
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
		// TODO: 在此处放置处理何时用“确定”来关闭
		//  对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用“取消”来关闭
		//  对话框的代码
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

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}
