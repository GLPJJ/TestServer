#ifndef GLP_CLIENTSOCKET_H_
#define GLP_CLIENTSOCKET_H_

#include "eventhandler.h"
#include "datablock.h"
#include "rwstream.h"
#include "mutex.h"

namespace Tool
{
	enum _Errorcode{
		//网络接受Buf异常
		EC_RECV_BUFFER,
		//数据流异常
		EC_STREAM,
	};

	class DataBlock;
	class DataProcessBase;

	class ClientUdpSocket : public FDEventHandler
	{
	public:
		ClientUdpSocket();
		ClientUdpSocket(Reactor *pReactor,const char* host,short port);
		virtual ~ClientUdpSocket(){}

	public:
		virtual void onFDWrite(){}
		virtual int Init();
		short getport(){return m_port;}

	private:
		short m_port;
		char m_host[20];
	};

	class ClientSocketBase;

	class OnSocketListener{
	public:
		// 连接成功
		virtual bool onSocketConnect(ClientSocketBase* client) = 0; 
		// 连接超时
		virtual void onSocketConnectTimeout(ClientSocketBase* client) = 0; 
		// 正常关闭(被动关闭),recv == 0的情况
		virtual void onSocketClose(ClientSocketBase* client) = 0; 
		// errcode为错误码(socket提供)
		virtual void onSocketConnectError(ClientSocketBase* client,int errCode) = 0; 
		virtual void onSocketRecvError(ClientSocketBase* client,int errCode) = 0; 
		virtual void onSocketSendError(ClientSocketBase* client,int errCode) = 0; 
		// 网络层错误(errCode网络层定义)
		virtual void onNetLevelError(ClientSocketBase* client,int errCode) = 0; 
	};

	class ClientSocketBase : public FDEventHandler
	{
	public:
		ClientSocketBase() : m_pDecoder(),m_bIsClosed(true),m_pListener(){
			m_pCSSendData = Mutex::CreateCriticalSection();
		}
		ClientSocketBase(Reactor *pReactor) : FDEventHandler(pReactor),m_pDecoder()
			,m_bIsClosed(true),m_pListener(){
			m_pCSSendData = Mutex::CreateCriticalSection();
		}
		virtual ~ClientSocketBase(){Mutex::Destroy(m_pCSSendData);}
        
	public:
		void reset();
		void setDecoder(DataProcessBase* pDecoder){m_pDecoder = pDecoder;}
		//网络可读的时候，recv数据
		virtual void onFDRead();
		//网络可写的时候，send数据
		virtual void onFDWrite();
		//关闭处理
		virtual void close();

		inline bool isClosed(){return m_bIsClosed;}
		DataBlock* getRB(){return &m_recvdata;}
		DataBlock* getWB(){return &m_senddata;}
		//add buffer的时候注册写fd_set，这样可以被执行到OnFDWrite();
		virtual int addBuf(WriteStream &stream);

		const char* getPeerIp();

		void setSocketListener(OnSocketListener* listener){m_pListener=listener;}

		//从域名中解析出Ip地址,只返回第一个解析出来的,字符串保存在静态空间中，返回值不需要释放！
		static const char* GetIpv4FromHostname(const char* name);
		//根据指定fd给ip地址
		static const char* GetPeerIp(int fd);
	protected:
		/*内部调用*/
		int addBuf(const char* buf,size_t buflen);
		void open(){m_bIsClosed = false;}
	public:
		// 连接成功
		virtual bool onSocketConnect() {if(m_pListener)return m_pListener->onSocketConnect(this);return true;} 
		// 连接超时
		virtual void onSocketConnectTimeout() {if(m_pListener)m_pListener->onSocketConnectTimeout(this);}
		// 正常关闭(被动关闭),recv == 0的情况
		virtual void onSocketClose() {if(m_pListener)m_pListener->onSocketClose(this);}
		// errcode为错误码(socket提供)
		virtual void onSocketConnectError(int errCode) {if(m_pListener)m_pListener->onSocketConnectError(this,errCode);}
		virtual void onSocketRecvError(int errCode) {if(m_pListener)m_pListener->onSocketRecvError(this,errCode);}
		virtual void onSocketSendError(int errCode) {if(m_pListener)m_pListener->onSocketSendError(this,errCode);}
		// 网络层错误(errCode网络层定义)
		virtual void onNetLevelError(int errCode) {if(m_pListener)m_pListener->onNetLevelError(this,errCode);}

	private:
		DataBlock m_recvdata;
		DataBlock m_senddata;
		DataProcessBase *m_pDecoder;
		bool m_bIsClosed;

		//当子类继承的时候，可以不用这个，它主要为服务器监听客户端socket提供接口
		OnSocketListener* m_pListener;
    public:
        Mutex* m_pCSSendData;
	};

	class ClientSocket : public ClientSocketBase ,public TMEventHandler
	{
	public:
		enum  eWaitType{
			wait_for_none,
			wait_for_connect,
			wait_for_write,
			wait_for_read
		};
	public:
		virtual ~ClientSocket(){}
		ClientSocket(Reactor *pReactor) : ClientSocketBase(pReactor),m_isConnected(false)
			,m_port(0),m_waitType(wait_for_none){ }

		//增加对非阻塞描述符的情况判断。
		virtual void onFDWrite();
		//连接超时
		virtual void onTimeOut();
		virtual void close();

		int connect(const char* host,short port,int to = 10);
		bool isConnect(){return m_isConnected;}
		//添加的sendbuf中，并注册到写fd_set中
		bool sendBuf(WriteStream &stream);

		inline const char* gethost(){return m_host;}
		inline short getport(){return m_port;}
	protected:
		bool sendBuf(const char* buf,size_t buflen);
	private:
		bool m_isConnected;
		char m_host[20];
		short m_port;

		eWaitType  m_waitType;
	};
};

#endif//GLP_CLIENTSOCKET_H_

